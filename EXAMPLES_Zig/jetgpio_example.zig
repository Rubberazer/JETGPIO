// Usage example of the JETGPIO library
// Compile with: zig build-exe jetgpio_example.zig -ljetgpio -lc
// Execute with: sudo ./jetgpio_example

const std = @import("std");
const jetgpio = @cImport({
    @cInclude("jetgpio.h");
});

pub fn main() !void {
    const init = jetgpio.gpioInitialise();

    if (init < 0) {
        std.debug.print("Error initiating jetgpio\n", .{});
    }
    // Setting up pin 3 as output
    const set1 = jetgpio.gpioSetMode(3, jetgpio.JET_OUTPUT);
    if (set1 < 0) {
        std.debug.print("Error setting pin 3\n", .{});
    }

    // Setting up pin 7 as input
    const set2 = jetgpio.gpioSetMode(7, jetgpio.JET_INPUT);
    if (set2 < 0) {
        std.debug.print("Error setting pin 7\n", .{});
    }

    // Writing 1 and 0 to pin 3 a 1 second intervals while reading pin 7
    var i: u8 = 0;
    var level: i32 = 0;
    var ret: i32 = 0;
    while (i < 5) {
        ret = jetgpio.gpioWrite(3, 1);
        std.time.sleep(1000);
        level = jetgpio.gpioRead(7);
        std.debug.print("level: {}\n", .{level});
        std.time.sleep(1000000000);
        ret = jetgpio.gpioWrite(3, 0);
        std.time.sleep(1000);
        level = jetgpio.gpioRead(7);
        std.debug.print("level: {}\n", .{level});
        std.time.sleep(1000000000);
        i += 1;
    }
    jetgpio.gpioTerminate();
}
