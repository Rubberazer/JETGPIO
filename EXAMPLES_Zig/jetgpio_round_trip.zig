// Usage example of the JETGPIO library
// Compile with: zig build-exe jetgpio_round_trip.zig -ljetgpio -lc
// Execute with: sudo ./jetgpio_round_trip

const std = @import("std");
const jetgpio = @cImport({
    @cInclude("jetgpio.h");
});

pub fn main() !void {
    const init = jetgpio.gpioInitialise();

    if (init < 0) {
        std.debug.print("Error initiating jetgpio\n", .{});
    }
    // Setting up pin 38 as output
    const set1 = jetgpio.gpioSetMode(38, jetgpio.JET_OUTPUT);
    if (set1 < 0) {
        std.debug.print("Error setting pin 38\n", .{});
    }

    // Setting up pin 40 as input
    const set2 = jetgpio.gpioSetMode(40, jetgpio.JET_INPUT);
    if (set2 < 0) {
        std.debug.print("Error setting pin 40\n", .{});
    }

    // Measuring time now
    const start = try std.time.Instant.now();

    // Writing 1 to pin 38
    _ = jetgpio.gpioWrite(38, 1);

    while (jetgpio.gpioRead(40) == 0) {}

    // Measuring time after the loop
    const end = try std.time.Instant.now();

    // Calculating time difference for round trip e.g. time invested in activating the output + the time it takes for the input to detect the level change
    const elapsed = end.since(start);
    std.debug.print("Elapsed time = {} nanoseconds\n", .{elapsed});

    // Writing 0 to pin 38
    _ = jetgpio.gpioWrite(38, 1);

    jetgpio.gpioTerminate();
}
