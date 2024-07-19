// Usage example of the JETGPIO library
// Compile with: zig build-exe jetgpio_PWM_example.zig -ljetgpio -lc
// Execute with: sudo ./jetgpio_PWM_example

const std = @import("std");
const jetgpio = @cImport({
    @cInclude("jetgpio.h");
});

pub fn main() !void {
    const init = jetgpio.gpioInitialise();

    if (init < 0) {
        std.debug.print("Error initiating jetgpio: {}\n", .{init});
        std.process.exit(1);
    }

    // Setting up PWM frequency=10kHz @ pin 32

    const PWMstat = jetgpio.gpioSetPWMfrequency(32, 10000);
    if (PWMstat < 0) {
        std.debug.print("PWM frequency set up failed. Error code:  {}\n", .{PWMstat});
        std.process.exit(1);
    }

    // Set up PWM duty cycle to approx 50% (0=0% to 256=100%) @ pin 32

    const PWMstat2 = jetgpio.gpioPWM(32, 128);

    if (PWMstat2 < 0) {
        std.debug.print("PWM start failed. Error code:  {}\n", .{PWMstat2});
        std.process.exit(1);
    }

    var x: i32 = 0;
    std.debug.print("PWM going at pin 32 for 60 seconds\n", .{});

    while (x < 30) {
        std.time.sleep(2000000000);
        x += 1;
    }

    // Terminating library
    jetgpio.gpioTerminate();
}
