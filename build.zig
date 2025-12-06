const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const mode = b.standardOptimizeOption(.{});

    const module = b.addModule("main", .{
        .target = target,
        .optimize = mode,
        .link_libc = true,
    });

    module.addCSourceFile(.{
        .file = b.path("src/main.c"),
        // zig fmt: off
         .flags = &[_][]const u8{ "-std=c17", "-Wall", "-Wextra", "-Werror", "-pedantic",
        "-Wshadow", "-Wconversion", "-Wdouble-promotion", "-Wformat=2", "-Wunused-macros", "-Wconversion",
        "-Og", "-g3", "-fno-omit-frame-pointer", "-fstack-protector-strong", "-DDEBUG", "-Wmissing-prototypes", "-Wmissing-prototypes", "-Wstrict-prototypes"}
        // zig fmt: on
    });

    module.linkSystemLibrary("raylib", .{});
    if (target.result.os.tag == .macos) {
        module.linkFramework("IOKit", .{});
        module.linkFramework("Cocoa", .{});
        module.linkFramework("OpenGL", .{});
    }

    const exe = b.addExecutable(.{
        .name = "game",
        .root_module = module,
    });

    b.installArtifact(exe);
    const run_cmd = b.addRunArtifact(exe);
    run_cmd.step.dependOn(b.getInstallStep());

    if (b.args) |args| {
        run_cmd.addArgs(args);
    }

    const run_step = b.step("run", "Run the game");
    run_step.dependOn(&run_cmd.step);
}
