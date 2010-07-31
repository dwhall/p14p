# Anything that starts with a # is a comment!
# Use lots of comments in your code to explain what you're doing.

# Use the PIC-specific Python libraries
import pic24_dspic33 as pic


###############################################################
# Set up line sensors, distance sensor, motors 
###############################################################

line_sensor_right =  pic.digital_io(1,   8,  True)
right_wheel_stopped  = 938

def drive_forward():
    right_wheel.setCounts(right_wheel_forward)
    
def stop_driving():
    right_wheel.setCounts(right_wheel_stopped)


###############################################################
# Main loop
###############################################################

while True:
    # Print out line following diags
    if line_sensor_right.get():
        pass    
