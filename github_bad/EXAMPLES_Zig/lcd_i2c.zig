// Usage example of the JETGPIO library
// Compile with: zig build-exe lcd_i2c.zig -ljetgpio -lc
// Execute with: sudo ./lcd_i2c

const std = @import("std");
const jetgpio = @cImport({
    @cInclude("jetgpio.h");
});

fn send_command(handle: u32, i2cAddr: u32, command: u32) void {
    var buffer: u32 = command & 0xF0;
    buffer |= 0x04;
    var buffer2: u32 = buffer;
    buffer |= 0x08;
    _ = jetgpio.i2cWriteByteData(handle, i2cAddr, buffer, 0x0);
    std.time.sleep(2000000);
    buffer2 &= 0xFB;
    buffer2 |= 0x08;
    _ = jetgpio.i2cWriteByteData(handle, i2cAddr, buffer2, 0x0);
    buffer = (command & 0x0F) << 4;
    buffer |= 0x04;
    buffer2 = buffer;
    buffer |= 0x08;
    _ = jetgpio.i2cWriteByteData(handle, i2cAddr, buffer, 0x0);
    std.time.sleep(2000000);
    buffer2 &= 0xFB;
    buffer2 |= 0x08;
    _ = jetgpio.i2cWriteByteData(handle, i2cAddr, buffer2, 0x0);
}

fn send_data(handle: u32, i2cAddr: u32, data: u32) void {
    var buffer: u32 = data & 0xF0;
    buffer |= 0x05;
    var buffer2: u32 = buffer;
    buffer |= 0x08;
    buffer2 &= 0xFB;
    buffer2 |= 0x08;
    _ = jetgpio.i2cWriteByteData(handle, i2cAddr, buffer, buffer2);
    buffer = (data & 0x0F) << 4;
    buffer |= 0x05;
    buffer2 = buffer;
    buffer |= 0x08;
    buffer2 &= 0xFB;
    buffer2 |= 0x08;
    _ = jetgpio.i2cWriteByteData(handle, i2cAddr, buffer, buffer2);
}

pub fn main() !void {
    const LCD_SLAVE_ADDRESS: u32 = 0x3f;
    std.debug.print("This will print something on a Freenove i2c 1602 LCD connected to pins 3,5 and it will stay until LCD reset\n", .{});

    const init = jetgpio.gpioInitialise();
    if (init < 0) {
        std.debug.print("Error initiating jetgpio, erro number: {}\n", .{init});
    }

    const lcd: i32 = jetgpio.i2cOpen(1, 0);
    if (lcd < 0) {
        std.debug.print("Error opening i2c port, error number: {}\n", .{lcd});
    }

    // Now setting stuff up, device has not register map, all characters are sent as is, not to a specific register address
    // the trick here is to send the same stuff twice for commands and for characters just once and then a 0, which does nothing
    send_command(@intCast(lcd), LCD_SLAVE_ADDRESS, 0x33);
    std.time.sleep(5000000);
    send_command(@intCast(lcd), LCD_SLAVE_ADDRESS, 0x32);
    std.time.sleep(5000000);
    send_command(@intCast(lcd), LCD_SLAVE_ADDRESS, 0x28);
    std.time.sleep(5000000);
    send_command(@intCast(lcd), LCD_SLAVE_ADDRESS, 0x0C);
    std.time.sleep(5000000);
    send_command(@intCast(lcd), LCD_SLAVE_ADDRESS, 0x01);
    std.time.sleep(5000000);
    _ = jetgpio.i2cWriteByteData(@intCast(lcd), LCD_SLAVE_ADDRESS, 0x08, 0x0);

    //Now can start writing to the lcd screen, starting at position 0,0 first line ot the left

    const message = "Jetgpio";
    const message2 = " by Rubberazer";

    send_command(@intCast(lcd), LCD_SLAVE_ADDRESS, 0x80); //Positioning cursor at point 0,0
    for (message) |i| {
        send_data(@intCast(lcd), LCD_SLAVE_ADDRESS, i);
    }

    send_command(@intCast(lcd), LCD_SLAVE_ADDRESS, 0xC0); //Positioning cursor at second line
    for (message2) |i| {
        send_data(@intCast(lcd), LCD_SLAVE_ADDRESS, i);
    }
    // Closing i2c connection
    _ = jetgpio.i2cClose(@intCast(lcd));
    jetgpio.gpioTerminate();
}
