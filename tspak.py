# -*- coding: utf-8 -*-

"""
TimeSplitters pak library
~~~~~~~~~~~~~~~~~~~~~~~~~

Work in progress.
"""

from pathlib import Path
import struct
import sys


class Pak:
    """Defines data in a TimeSplitters pak file."""

    _fh = None
    _format = None
    _path = None
    dir_offset = None
    dir_length = None
    dir_count = None
    name_offset = None

    def __init__(self, path):
        """Initialize a pak file."""
        fh = path.open("rb")
        pak_type = fh.read(4).decode("utf-8")
        self._fh = fh
        self._path = path

        if pak_type in ("P4CK", "P5CK"):
            header_keys = ("dir_offset", "dir_length")
            header_fmt = "<II"
        elif pak_type == "P8CK":
            header_keys = ("dir_offset", "dir_count", "name_offset")
            header_fmt = "<III"

        if pak_type == "P4CK":
            fat_keys = ("name", "offset", "length", "unk")
            fat_fmt = "<48sIII"
        elif pak_type == "P5CK":
            fat_keys = ("id", "offset", "length", "unk")
            fat_fmt = "<IIII"
        elif pak_type == "P8CK":
            fat_keys = ("unk", "length", "offset")
            fat_fmt = "<LLL"

        # Setting header and entry formats
        header = struct.Struct(header_fmt)
        fat = struct.Struct(fat_fmt)

        # Get header data
        header_data = fh.read(header.size)
        header_values = header.unpack_from(header_data)
        for key, value in zip(header_keys, header_values):
            setattr(self, key, value)

        # Calculate num entries for P4/P5
        if not self.dir_count:
            self.dir_count = self.dir_length / fat.size

        self.files = []
        fh.seek(self.dir_offset)
        while len(self.files) < self.dir_count:
            fat_data = fh.read(fat.size)
            fat_values = fat.unpack_from(fat_data)
            self.files.append(dict(zip(fat_keys, fat_values)))

        # Add names for P8CK
        if self.name_offset:
            names = []
            fh.seek(self.name_offset)
            data = b""
            while len(names) < self.dir_count:
                data += fh.read()
                maxsplit = self.dir_count - len(names)
                new_names = data.split(b"\0", maxsplit=maxsplit)
                data = new_names.pop()
                names.extend(new_names)
                if len(names) > self.dir_count:
                    del names[self.dir_count :]
            for num in range(self.dir_count):
                self.files[num]["name"] = names[num]

        # Get clean filenames
        for entry in self.files:
            if "id" in entry:
                entry["filename"] = str(entry["id"])
            else:
                entry["filename"] = entry["name"].decode("utf-8").strip("\x00")

    def extract(self):
        """Extract the whole pak file."""
        # target dir
        path = self._path
        target = path.parent / path.stem

        # extract
        for entity in self.files:
            # filename = Path(f"{entity['filename']}[{entity['unk']:X}]")
            filename = Path(entity["filename"])
            tree = target / filename
            print(f"Creating {str(tree)}...")
            tree.parent.mkdir(parents=True, exist_ok=True)
            with tree.open("wb") as fh:
                self._fh.seek(entity["offset"])
                buf = b""
                while len(buf) < entity["length"]:
                    buf += self._fh.read()
                fh.write(buf[: entity["length"]])


def pak_extract(path):
    """Extract a pak file."""
    pak_file = Path(path)
    pak = Pak(pak_file)
    pak.extract()


if __name__ == "__main__":
    args = sys.argv[1:]
    if not args:
        print(f"Usage: {sys.argv[0]} file1.pak file2.pak...")

    for arg in args:
        pak_extract(arg)
