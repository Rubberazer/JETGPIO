// Usage example of the JETGPIO library
// Compile with: zig build-exe jetgpio_edge.zig -ljetgpio -lc
// Execute with: sudo ./jetgpio_edge

const std = @import("std");
const jetgpio = @cImport({
    @cInclude("jetgpio.h");
});
const c = @cImport({
    @cInclude("signal.h");
});

// Global variable to interrupt the loop later on
var interrupt: bool = true;
// Global variable timestamp
var timestamp: u64 = 0;

// Ctrl-c signal function handler
fn inthandler(_: c_int) callconv(.C) void {
    std.time.sleep(1000000);
    std.debug.print("\nCaught Ctrl-c, coming out ...\n", .{});
    interrupt = false;
}

// Function to be called upon if edge is detected
fn calling(...) callconv(.C) void {
    std.debug.print("edge detected with EPOCH timestamp: {}\n", .{timestamp});
}

pub fn main() !void {

    // Capture Ctrl-c
    _ = c.signal(c.SIGINT, &inthandler);

    const init = jetgpio.gpioInitialise();

    if (init < 0) {
        std.debug.print("Error initiating jetgpio\n", .{});
    }

    // Setting up pin 3 as input
    const set1 = jetgpio.gpioSetMode(3, jetgpio.JET_INPUT);
    if (set1 < 0) {
        std.debug.print("Error setting pin 3\n", .{});
    }

    // Now setting up pin 3 to detect edges, rising & falling edge with a 1000 useconds debouncing and when event is detected calling func "calling"
    _ = jetgpio.gpioSetISRFunc(3, jetgpio.EITHER_EDGE, 1000, &timestamp, calling);

    while (interrupt) {
        std.time.sleep(1000000000);
    }

    // Terminating library
    jetgpio.gpioTerminate();
}
