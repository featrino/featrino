![alt text](resources/icon-64.png)
featrino
============

A fork of [chatterino2](https://github.com/Chatterino/chatterino2/) with the aim of allow more features.
Goal is to always keep up to date with the master, while also including any feature users want and works.
Feel free to make a PR and will be merged if it works.
If the feature breaks, it will be reverted to ensure that it doesn't fall behind in functionality.


## Features (todo)

- Longer message history (done)
- Faster startup for live status (done)
- 4x emote support for FFZ (done)
- 7tv emote and animated profiles (done, taken from [chatterino7](https://github.com/SevenTV/chatterino7)) 


## Ideas

- Built in video player (MPV+streamlink) for viewing stream
- Live preview of stream, or more recent stream previews on hover
- Notes for users (better implementation of [this](https://github.com/Chatterino/chatterino2/pull/3745))


## Building

- [Building on Windows](./docs/BUILDING_ON_WINDOWS.md)
- [Building on Windows with vcpkg](./docs/BUILDING_ON_WINDOWS_WITH_VCPKG.md)
- [Building on Linux](./docs/BUILDING_ON_LINUX.md)
- [Building on Mac](./docs/BUILDING_ON_MAC.md)
- [Building on FreeBSD](./docs/BUILDING_ON_FREEBSD.md)

```
git clone https://github.com/featrino/featrino.git
cd featrino
git submodule update --init --recursive
```


## License

As with the original repo all work is under the MIT license.
Thus you are free to do what you want as long as you state its copyright notice.

