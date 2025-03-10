#Enable PWM clocks on Xavier
#!/bin/bash

bash -c 'echo "1" > /sys/kernel/debug/bpmp/debug/clk/pwm1/state'
bash -c 'echo "1" > /sys/kernel/debug/bpmp/debug/clk/pwm8/state'
