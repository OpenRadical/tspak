# tspak

tspak is a small utility program designed to extract the contents of TimeSplitters `.pak` files.

## PAK formats

tspak supports the following formats:

* `P4CK` - Timesplitters 1 & 2 (PS2)
* `P5CK` - TimeSplitters: Future Perfect
* `P8CK` - TimeSplitters 2 (GameCube / XBox)

## Usage

```sh
tspak file1.pak [file2.pak...]
```

Drag and drop is naturally supported. Drag a bunch of PAK files onto the executable to extract them. If you are extracting TS:FP pak files, make sure the `.c2n` checksum file exists at the same path.

## Note about HomeFront 2

This extractor works on TimeSplitters `.pak` files only. If you are hoping to modify files in the HomeFront 2 port of TimeSplitters 2, you must first extract `lsao_cached.pak` using [hf2decrypt] or a similar utility first. `tspak` can be used on the XBox format `.pak` files contained within!

[hf2decrypt]: https://files.timesplitters.dev/m0xf/hf2decrypt.zip
