# GOcontroll-Simulink

The MATLAB Simulink blockset for the GOcontroll Moduline controllers: the block libraries,
the Embedded Coder target and two model templates.

| Controller | Platform |
|---|---|
| Moduline S1 | STM32, FreeRTOS |
| Moduline M1, L4, HMI1 | Linux (ARM64) |

This repository is the blockset only. **To start a project, use
[GOcontroll-Simulink-Starter](https://github.com/GOcontroll/GOcontroll-Simulink-Starter)**: a
ready project folder that carries this repository as a git submodule. Your models stay in your
own repository and you update the blockset with one git command.

Documentation: the [GOcontroll knowledge base](https://gocontroll.com/knowledge-base/).

## What you need

- **MATLAB R2023b or later**, with Simulink, Embedded Coder, MATLAB Coder and Simulink Coder.
  The libraries and templates are saved in R2023b.
- **Git.**
- **A compiler** for the controller. `GOcontrollInstallCompilers` in the MATLAB Command Window
  downloads and installs the tested versions for the current user, where the build finds them
  without any `PATH` setting:

  | For | Command | Compiler | Name the blockset calls |
  |---|---|---|---|
  | Moduline S1 | `GOcontrollInstallCompilers S1` | Arm GNU Toolchain arm-none-eabi 14.2.rel1 | `arm-none-eabi-gcc` |
  | Moduline M1, L4, HMI1 (*Compiler* = GCC) | `GOcontrollInstallCompilers Linux` | GCC 9.2-2019.12 aarch64-none-linux-gnu | `aarch64-none-linux-gnu-gcc` |
  | Moduline M1, L4, HMI1 (*Compiler* = Zig) | `GOcontrollInstallCompilers Zig` | Zig 0.14.1 | `zig` |

  A compiler is looked up in this order: a folder filled in in
  `blockset/utility_functions/getInstallationLocation.m`, then the compilers installed with
  `GOcontrollInstallCompilers` (`%LOCALAPPDATA%\GOcontroll\tc` on Windows,
  `~/.gocontroll/tc` elsewhere), then the system `PATH`.

## Using the blockset in a project

The recommended layout keeps the blockset in a subfolder of your project:

```
MyProject/                      your own repository
├── MyModel.slx                 your models
├── startup.m                   puts the blockset on the MATLAB path
├── librarySetup.m              forwards to the blockset
└── GOcontroll-Simulink/        this repository, as a submodule
    └── blockset/
        └── GOcontroll-CodeBase/    the C code base, a submodule of this repository
```

[GOcontroll-Simulink-Starter](https://github.com/GOcontroll/GOcontroll-Simulink-Starter) is
exactly this layout. To add the blockset to an existing repository instead:

```
git submodule add https://github.com/GOcontroll/GOcontroll-Simulink.git GOcontroll-Simulink
git submodule update --init --recursive
```

and copy `startup.m` and `librarySetup.m` from the starter into the root of your project.

Then:

1. Start MATLAB in the project folder. `startup.m` puts the blockset on the path.
2. *New > Simulink Model*, group **GOcontroll**: choose the template **GOcontroll Moduline S1**
   or **GOcontroll Moduline M1 / L4 / HMI1**, and save the model in the project folder.
3. Choose the controller under *Configuration Parameters > Code Generation > GOcontroll
   options > Target*.
4. Build (Ctrl+B) with the project folder as MATLAB's current folder.

A build writes `<model>_<major>.<minor>.<patch>` with the extensions `.elf`, `.a2l` and `.map`
(Moduline S1 also `.srec` and `.bin`) into `build/` next to the model. The version comes from
the **Software version** block in the model.

For the Linux controllers, `ManualUpload <model> <controller address>` in the MATLAB command
window sends the newest `.elf` and `.a2l` to the controller.

## Updating the blockset

In a project that carries this repository as a submodule:

```
git submodule update --remote GOcontroll-Simulink
git -C GOcontroll-Simulink submodule update --init --recursive
```

The first command fetches the newest blockset; the second puts the code base inside it on the
version that belongs to that blockset. Restart MATLAB afterwards. Your project records which version of the blockset it uses; commit the changed submodule
together with your models once they build with the new version.

## Cloning this repository on its own

```
git clone --recurse-submodules https://github.com/GOcontroll/GOcontroll-Simulink.git
```

The blockset relies on [GOcontroll-CodeBase](https://github.com/GOcontroll/GOcontroll-CodeBase),
which is a submodule in `blockset/GOcontroll-CodeBase`. Without `--recurse-submodules`, run
`git submodule update --init --recursive` afterwards.

## Earlier versions

Blockset 4.x is on the branch [`gps`](https://github.com/GOcontroll/GOcontroll-Simulink/tree/gps)
(release V4.6.0). Version 5 has a different folder layout and one target for all controllers;
a model made with 4.x has to be moved to a new model made from a template.

## License

MIT, see [LICENSE](LICENSE). The model templates carry a disclaimer: read it before use.
