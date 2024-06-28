// Usage example of the JETGPIO library
// Compile with: zig build-exe jetgpio_output.zig -ljetgpio -lc
// Execute with: sudo ./jetgpio_output

const std = @import("std");
const jetgpio = @cImport({
    @cInclude("jetgpio.h");
});
const c = @cImport({
    @cInclude("signal.h");
});

// Global variable to interrupt the loop later on
var interrupt: bool = true;

// Ctrl-c signal function handler
fn inthandler(_: c_int) callconv(.C) void {
    std.time.sleep(1000000);
    std.debug.print("\nCaught Ctrl-c, coming out ...\n", .{});
    interrupt = false;
}

pub fn main() !void {

    // Capture Ctrl-c
    _ = c.signal(c.SIGINT, &inthandler);

    const init = jetgpio.gpioInitialise();

    if (init < 0) {
        std.debug.print("Error initiating jetgpio\n", .{});
    }

    // Setting up pin 38 as output
    const set1 = jetgpio.gpioSetMode(38, jetgpio.JET_OUTPUT);
    if (set1 < 0) {
        std.debug.print("Error setting pin 38\n", .{});
    }

    var ret: i32 = 0;

    while (interrupt) {
        ret = jetgpio.gpioWrite(38, 1);
        ret = jetgpio.gpioWrite(38, 0);
    }

    // Pin 38 at 0
    ret = jetgpio.gpioWrite(38, 0);

    // Terminating library
    jetgpio.gpioTerminate();
}
