#Enable PWM clocks on Orin
#!/bin/bash

bash -c 'echo "1" > /sys/kernel/debug/bpmp/debug/clk/pwm1/state'
bash -c 'echo "1" > /sys/kernel/debug/bpmp/debug/clk/pwm5/state'
bash -c 'echo "1" > /sys/kernel/debug/bpmp/debug/clk/pwm7/state'
bash -c 'echo "1" > /sys/kernel/debug/bpmp/debug/clk/pwm8/state'
