# osu-memory

Yet another memory reader for osu! in C++.

This project is under consideration.

## Motivation

To rediscover my childhood.

## Use

```cpp
#include <osu_memory/osu_memory.h>

osu_memory::reader r;
r.get_hit_miss();
r.get_hit_50();
r.get_hit_100();
r.get_hit_200();
r.get_hit_300();
r.get_hit_perfect();
r.get_hit_geki();
r.get_hit_katsu();
r.get_mania_keys();
r.get_mod();
if (*(r.get_mod()) & mod_t::no_fail);
```

## License

Copyright (c) UnnamedOrange. Licensed under the MIT License.

See the [LICENSE](./LICENSE) file in the repository root for full license text.

## Special Thanks

- [l3lackShark](https://github.com/l3lackShark) and his [gosumemory](https://github.com/l3lackShark/gosumemory) for most of the memory signatures.
- [Piotrekol](https://github.com/Piotrekol/) and his [ProcessMemoryDataFinder](https://github.com/Piotrekol/ProcessMemoryDataFinder) for technical support.

## Credits

