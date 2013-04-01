/*
* Copyright (c) 2007-2009 SlimDX Group
* 
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* 
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*/
using System;
using System.Globalization;
using System.Windows.Forms;
using SlimDX;
using SlimDX.DirectInput;
using System.Collections.Generic;

namespace JoystickTest
{
    public partial class MainForm : Form
    {
        Joystick joystick;

        JoystickState state = new JoystickState();
        int numPOVs;
        int SliderCount;

        void CreateDevice()
        {
            // make sure that DirectInput has been initialized
            DirectInput dinput = new DirectInput();

            // search for devices
            foreach (DeviceInstance device in dinput.GetDevices(DeviceClass.GameController, DeviceEnumerationFlags.AttachedOnly))
            {
                // create the device
                try
                {
                    joystick = new Joystick(dinput, device.InstanceGuid);
                    joystick.SetCooperativeLevel(this, CooperativeLevel.Exclusive | CooperativeLevel.Foreground);
                    break;
                }
                catch (DirectInputException)
                {
                }
            }

            if (joystick == null)
            {
                MessageBox.Show("There are no joysticks attached to the system.");
                return;
            }

            foreach (DeviceObjectInstance deviceObject in joystick.GetObjects())
            {
                
                if ((deviceObject.ObjectType & ObjectDeviceType.Axis) != 0)
                {
                    joystick.GetObjectPropertiesById((int)deviceObject.ObjectType).SetRange(0, 255);
                }
 
                
                UpdateControl(deviceObject);
            }

            // acquire the device
            joystick.Acquire();

            // set the timer to go off 12 times a second to read input
            // NOTE: Normally applications would read this much faster.
            // This rate is for demonstration purposes only.
            timer.Interval = 5; // 1000 / 1000;
            timer.Start();
        }

        void ReadImmediateData()
        {
            if (joystick.Acquire().IsFailure)
                return;

            if (joystick.Poll().IsFailure)
                return;

            state = joystick.GetCurrentState();

            if (Result.Last.IsFailure)
                return;

            UpdateUI();  // new Data

        }

        void ReleaseDevice()
        {
            timer.Stop();

            if (joystick != null)
            {
                joystick.Unacquire();
                joystick.Dispose();
            }
            joystick = null;
        }

        #region Boilerplate

        public MainForm()
        {
            InitializeComponent();
            UpdateUI();
        }

        private void timer_Tick(object sender, EventArgs e)
        {
            ReadImmediateData();
        }

        private void exitButton_Click(object sender, EventArgs e)
        {
            ReleaseDevice();
            Close();
        }

        void UpdateUI()
        {
            String[] incomingDataFromUSB = new String[7]; //usb den gelen 7 bytelýk paketimiz


            if (joystick == null)
            {
                createDeviceButton.Text = "Create Device";
            }
            else
            {
                createDeviceButton.Text = "Release Device";
            }

            string strText = null;

            label_X.Text = state.X.ToString(CultureInfo.CurrentCulture);
            label_Y.Text = state.Y.ToString(CultureInfo.CurrentCulture);
            label_Z.Text = state.Z.ToString(CultureInfo.CurrentCulture);

            label_XRot.Text = state.RotationX.ToString(CultureInfo.CurrentCulture);
            label_YRot.Text = state.RotationY.ToString(CultureInfo.CurrentCulture);
            label_ZRot.Text = state.RotationZ.ToString(CultureInfo.CurrentCulture);

       

            int[] slider = state.GetSliders();

            int[] test1 = state.GetAccelerationSliders();
            int[] test2 = state.GetForceSliders();
            int[] test3 = state.GetVelocitySliders();
            String test4 = state.ToString();

            label_S0.Text = slider[0].ToString(CultureInfo.CurrentCulture);

            label_S1.Text = slider[1].ToString(CultureInfo.CurrentCulture);

            int[] pov = state.GetPointOfViewControllers();

            label_P0.Text = pov[0].ToString(CultureInfo.CurrentCulture);
            label_P1.Text = pov[1].ToString(CultureInfo.CurrentCulture);
            label_P2.Text = pov[2].ToString(CultureInfo.CurrentCulture);
            label_P3.Text = pov[3].ToString(CultureInfo.CurrentCulture);

            bool[] buttons = state.GetButtons();

            for (int b = 0; b < buttons.Length; b++)
            {
                if (buttons[b])
                    strText += b.ToString("00 ", CultureInfo.CurrentCulture);
            }
            label_ButtonList.Text = strText;



            //fs
            incomingDataFromUSB[0] = slider[0].ToString(CultureInfo.CurrentCulture); //index ilk byte.
            incomingDataFromUSB[1] = state.X.ToString(CultureInfo.CurrentCulture); //byte1
            incomingDataFromUSB[2] = state.Y.ToString(CultureInfo.CurrentCulture); //byte1
            incomingDataFromUSB[3] = state.Z.ToString(CultureInfo.CurrentCulture); //byte1
            incomingDataFromUSB[4] = state.RotationX.ToString(CultureInfo.CurrentCulture); //byte1
            incomingDataFromUSB[5] = state.RotationY.ToString(CultureInfo.CurrentCulture); //byte1
            incomingDataFromUSB[6] = state.RotationZ.ToString(CultureInfo.CurrentCulture); //byte1

            fs_dataProcessStateMachine(incomingDataFromUSB);

        }

        private static int fs_state = 0; //machine durumumuz
        private String[] dataAll = new String[18];

        void fs_dataProcessStateMachine(String[] data)
        {
            switch (fs_state)
            {
                case 0:
                    if (data[0] == "1") //1 indexli datanýn ilk kýsmý geldi.  ilk kýsmý al ve 1. state egeç
                    {
                        dataAll[0] = data[1];
                        dataAll[1] = data[2];
                        dataAll[2] = data[3];
                        dataAll[3] = data[4];
                        dataAll[4] = data[5];
                        dataAll[5] = data[6];
                        
                        fs_state = 1;
                    }
                    else //baþka biþey geldiyse yanlýþ data. baþlangýç stateine geri dön
                    {
                        fs_state = 0;
                    }
                    break;
                
                case 1:
                    if (data[0] == "2") ////2 indexli datanýn ikinci kýsmý geldi.  ikinci kýsmý al ve 2. state egeç
                    {
                        dataAll[6] = data[1];
                        dataAll[7] = data[2];
                        dataAll[8] = data[3];
                        dataAll[9] = data[4];
                        dataAll[10] = data[5];
                        dataAll[11] = data[6];

                        fs_state = 2;
                    }
                    else if (data[0] == "1") //bir önceki statin datasý tekrar geldi bekle
                    {
                    }
                    else //baþka biþey geldiyse yanlýþ data. baþlangýç stateine geri dön
                    {
                        fs_state = 0;
                    }
                    break;
                
                case 2:
                    if (data[0] == "3") ////3 indexli datanýn son kýsmýda geldi.  data tamamlandý ekrana yaz. tekrar 1. state egeç
                    {
                        dataAll[12] = data[1];
                        dataAll[13] = data[2];
                        dataAll[14] = data[3];
                        dataAll[15] = data[4];
                        dataAll[16] = data[5];
                        dataAll[17] = data[6];

                        fs_state = 0;

                        listBox1.Items.Add(dataAll.ToString());
                    }
                    else if (data[0] == "2") //bir önceki statin datasý tekrar geldi bekle
                    {
                    }
                    else //baþka biþey geldiyse yanlýþ data. baþlangýç stateine geri dön
                    {
                        fs_state = 0;
                    }
                    break;
                
                default:
                    fs_state = 0;
                    break;

            }
        }

        void UpdateControl(DeviceObjectInstance d)
        {
            if (ObjectGuid.XAxis == d.ObjectTypeGuid)
            {
                label_XAxis.Enabled = true;
                label_X.Enabled = true;
            }
            if (ObjectGuid.YAxis == d.ObjectTypeGuid)
            {
                label_YAxis.Enabled = true;
                label_Y.Enabled = true;
            }
            if (ObjectGuid.ZAxis == d.ObjectTypeGuid)
            {
                label_ZAxis.Enabled = true;
                label_Z.Enabled = true;
            }
            if (ObjectGuid.RotationalXAxis == d.ObjectTypeGuid)
            {
                label_XRotation.Enabled = true;
                label_XRot.Enabled = true;
            }
            if (ObjectGuid.RotationalYAxis == d.ObjectTypeGuid)
            {
                label_YRotation.Enabled = true;
                label_YRot.Enabled = true;
            }
            if (ObjectGuid.RotationalZAxis == d.ObjectTypeGuid)
            {
                label_ZRotation.Enabled = true;
                label_ZRot.Enabled = true;
            }

            if (ObjectGuid.Slider == d.ObjectTypeGuid)
            {
                switch (SliderCount++)
                {
                    case 0:
                        label_Slider0.Enabled = true;
                        label_S0.Enabled = true;
                        break;

                    case 1:
                        label_Slider1.Enabled = true;
                        label_S1.Enabled = true;
                        break;
                }
            }

            if (ObjectGuid.PovController == d.ObjectTypeGuid)
            {
                switch (numPOVs++)
                {
                    case 0:
                        label_POV0.Enabled = true;
                        label_P0.Enabled = true;
                        break;

                    case 1:
                        label_POV1.Enabled = true;
                        label_P1.Enabled = true;
                        break;

                    case 2:
                        label_POV2.Enabled = true;
                        label_P2.Enabled = true;
                        break;

                    case 3:
                        label_POV3.Enabled = true;
                        label_P3.Enabled = true;
                        break;
                }
            }
        }

        private void createDeviceButton_Click(object sender, EventArgs e)
        {
            if (joystick == null)
                CreateDevice();
            else
                ReleaseDevice();
            UpdateUI();
        }

        private void MainForm_FormClosed(object sender, FormClosedEventArgs e)
        {
            ReleaseDevice();
        }

        #endregion
    }
}