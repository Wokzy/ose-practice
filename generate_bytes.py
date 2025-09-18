
import argparse

parser = argparse.ArgumentParser(formatter_class=argparse.ArgumentDefaultsHelpFormatter)

parser.add_argument("--size", type=int, default=256)
parser.add_argument("--output_fname", type=str, default='foo.bin')

args = parser.parse_args()
out = bytearray([((args.size // 512) & 0xff) + (args.size % 512 != 0), (args.size // 512) >> 8])

TEMPLATE = b'\xFA'

with open(args.output_fname, 'wb') as f:
	f.write(out + TEMPLATE * args.size)
