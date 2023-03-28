from inputs import get_gamepad
import math
import threading
import os
# https://stackoverflow.com/questions/46506850/how-can-i-get-input-from-an-xbox-one-controller-in-python

class XboxController(object):
    # Distance from 0 where joysticks will return 0, 
    # stops stick drift/unintentional movements.
    deadzoneval = 0.3

    # Constants dont change, used to normalise 
    # joystick values between -1 and 1, e.g trig_val/MAX_TRIG_VAL
    MAX_TRIG_VAL = math.pow(2, 8)
    MAX_JOY_VAL = math.pow(2, 15)

    def __init__(self):

        self.LeftJoystickY = 0
        self.LeftJoystickX = 0
        self.RightJoystickY = 0
        self.RightJoystickX = 0
        self.LeftTrigger = 0
        self.RightTrigger = 0
        self.LeftBumper = 0
        self.RightBumper = 0
        self.A = 0
        self.X = 0
        self.Y = 0
        self.B = 0
        self.LeftThumb = 0
        self.RightThumb = 0
        self.Back = 0
        self.Start = 0
        self.LeftDPad = 0
        self.RightDPad = 0
        self.UpDPad = 0
        self.DownDPad = 0

        self._monitor_thread = threading.Thread(target=self._monitor_controller, args=())
        self._monitor_thread.daemon = True
        self._monitor_thread.start()

    # Written for project
    # Prevent stick drift, rounds controller values within deadzone to 0.
    def deadzone(self, value):
        if(not (value>-XboxController.deadzoneval and value<XboxController.deadzoneval)):
            return True
        return False
        
    # Written for project
    def read(self): # return the buttons/triggers that you care about.

        # For movement
        leftTrig = 1 if self.RightTrigger > 0.5 else 0
        rightTrig = 1 if self.LeftTrigger > 0.5 else 0

        r = self.RightDPad
        l = -self.LeftDPad
        horiz = r+l;
        horiz = 0 if horiz == 2 else horiz

        
        u = self.UpDPad
        d = -self.DownDPad
        vert = u+d;
        vert = 0 if vert == 2 else vert
        
        return [leftTrig,rightTrig,horiz,vert,self.LeftBumper,self.RightBumper]


    def _monitor_controller(self):
        while True:
            events = get_gamepad()
            for event in events:
                if event.code == 'ABS_Y':
                    self.LeftJoystickY = event.state / XboxController.MAX_JOY_VAL # normalize between -1 and 1
                elif event.code == 'ABS_X':
                    self.LeftJoystickX = event.state / XboxController.MAX_JOY_VAL # normalize between -1 and 1
                elif event.code == 'ABS_RY':
                    self.RightJoystickY = event.state / XboxController.MAX_JOY_VAL # normalize between -1 and 1
                elif event.code == 'ABS_RX':
                    self.RightJoystickX = event.state / XboxController.MAX_JOY_VAL # normalize between -1 and 1
                elif event.code == 'ABS_Z':
                    self.LeftTrigger = event.state / XboxController.MAX_TRIG_VAL # normalize between 0 and 1
                elif event.code == 'ABS_RZ':
                    self.RightTrigger = event.state / XboxController.MAX_TRIG_VAL # normalize between 0 and 1
                elif event.code == 'BTN_TL':
                    self.LeftBumper = event.state
                elif event.code == 'BTN_TR':
                    self.RightBumper = event.state
                elif event.code == 'BTN_SOUTH':
                    self.A = event.state
                elif event.code == 'BTN_NORTH':
                    self.Y = event.state #previously switched with X
                elif event.code == 'BTN_WEST':
                    self.X = event.state #previously switched with Y
                elif event.code == 'BTN_EAST':
                    self.B = event.state
                elif event.code == 'BTN_THUMBL':
                    self.LeftThumb = event.state
                elif event.code == 'BTN_THUMBR':
                    self.RightThumb = event.state
                elif event.code == 'BTN_SELECT':
                    self.Back = event.state
                elif event.code == 'BTN_START':
                    self.Start = event.state
                elif event.code == 'BTN_TRIGGER_HAPPY1':
                    self.LeftDPad = event.state
                elif event.code == 'BTN_TRIGGER_HAPPY2':
                    self.RightDPad = event.state
                elif event.code == 'BTN_TRIGGER_HAPPY3':
                    self.UpDPad = event.state
                elif event.code == 'BTN_TRIGGER_HAPPY4':
                    self.DownDPad = event.state

if __name__ == "__main__":
    joy = XboxController()
    last = joy.read()
    while(True):
        if(last != joy.read()):
            last=joy.read();
            os.system('cls')
            print(joy.read())