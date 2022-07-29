![alt text](resources/icon-64.png)
featrino
============

A fork of [chatterino2](https://github.com/Chatterino/chatterino2/) with the aim of allow more features.
Goal is to always keep up to date with the master, while also including any feature users want and works.
Feel free to make any feature PR and it will be merged if it works.
If the feature breaks, it will be reverted to ensure that the project doesn't fall behind master.


## Features

- Longer message history for user logs
- Faster startup for live status
- 4x emote support for FFZ and 7TV
- 7tv emote and animated profiles (from [chatterino7](https://github.com/SevenTV/chatterino7) commit [c74a082](https://github.com/SevenTV/chatterino7/commits/c74a0824e491e6f488f5f3ba5a83b89c4fded2e1)) 
- Support for pinning of user popups



## Future Ideas

- Live preview of stream, or more recent stream previews on hover
- Notes for users (better implementation of [this](https://github.com/Chatterino/chatterino2/pull/3745), use tabs)
- Spellcheck (english only) for messages and notes
- Support for prediction gamba when they happen (badges are already shown)
- List top 20 emotes that a user uses (in emote menu or `:` dialog)



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

