# osu-memory

Yet another memory reader for osu! in C++.

This project is under consideration.

## Motivation

To rediscover my childhood.

## Use

```cpp
#include <osu_memory/osu_memory.h>
using namespace osu_memory;
auto t1 = reader::get_50();
auto t2 = reader::get_100();
auto t3 = reader::get_200();
auto t4 = reader::get_300();
auto t5 = reader::get_perfect();
```

## License

Copyright (c) UnnamedOrange. Licensed under the MIT License.

See the [LICENSE](./LICENSE) file in the repository root for full license text.

## Special Thanks

- [l3lackShark](https://github.com/l3lackShark) and his [gosumemory](https://github.com/l3lackShark/gosumemory) for most of the memory signatures.
- [Piotrekol](https://github.com/Piotrekol/) and his [ProcessMemoryDataFinder](https://github.com/Piotrekol/ProcessMemoryDataFinder) for technical support.

## Credits

