// Usage example of the JETGPIO library
// Compile with: zig build-exe spi_loop.zig -ljetgpio -lc
// Execute with: sudo ./spi_loop

const std = @import("std");
const jetgpio = @cImport({
    @cInclude("jetgpio.h");
});

pub fn main() !void {
    const txc = [_]u8{ 0xff, 0xaa, 0xbb, 0xcc, 0xdd, 0xff, 0x11 };
    const tx = &txc;
    var rxc = [_]u8{ 0, 0, 0, 0, 0, 0, 0 };
    var rx = &rxc;
    const init = jetgpio.gpioInitialise();
    if (init < 0) {
        std.debug.print("Error initiating jetgpio\n", .{});
    }

    //Port SPI2 pins: 37, 22, 13 & 18
    //to perform a simple loop test, pins 22: SPI2_MISO & 37:SPI2_MOSI should be connected
    //with a short jumper cable, when the cable is disconnected the output on screen will show
    //all the rx values as zeros
    //spiOpen() parameters go as follows: spiOpen(port number, speed in Hz, mode, cs pin delay in us,
    //bits per word, least significant bit first, cs change)

    const spiInit = jetgpio.spiOpen(1, 5000000, 0, 0, 8, 1, 1);
    if (spiInit < 0) {
        std.debug.print("Port SPI2 opening failed. Error code: {}\n", .{spiInit});
    } else {
        std.debug.print("Port SPI2 opened OK. Return code: {}\n", .{spiInit});
    }

    //Transfer data
    var i: i8 = 0;
    while (i < 20) {
        // Here some weird casting, expected something more straightforward but...oh well, very strict type control and all that
        // also, this is interacting with a C abi after all! more than good enough
        const set2 = jetgpio.spiXfer(@intCast(spiInit), @constCast(tx.ptr), rx.ptr, 7);
        if (set2 < 0) {
            std.debug.print("Spi transfer failed. Error code: {}\n", .{set2});
        } else {
            std.debug.print("Spi transfer OK. Return code: {}\n", .{set2});
        }

        std.debug.print("tx0:{x} --> rx0:{x}\n", .{ tx[0], rx[0] });
        std.debug.print("tx1:{x} --> rx1:{x}\n", .{ tx[1], rx[1] });
        std.debug.print("tx2:{x} --> rx2:{x}\n", .{ tx[2], rx[2] });
        std.debug.print("tx3:{x} --> rx3:{x}\n", .{ tx[3], rx[3] });
        std.debug.print("tx4:{x} --> rx4:{x}\n", .{ tx[4], rx[4] });
        std.debug.print("tx5:{x} --> rx5:{x}\n", .{ tx[5], rx[5] });
        std.debug.print("tx6:{x} --> rx6:{x}\n", .{ tx[6], rx[6] });

        rx[0] = 0;
        rx[1] = 0;
        rx[2] = 0;
        rx[3] = 0;
        rx[4] = 0;
        rx[5] = 0;
        rx[6] = 0;

        i += 1;
        std.time.sleep(1000000000);
    }

    // Closing spi port
    _ = jetgpio.spiClose(@intCast(spiInit));
    jetgpio.gpioTerminate();
}
