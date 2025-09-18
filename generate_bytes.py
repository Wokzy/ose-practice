
import argparse

parser = argparse.ArgumentParser(formatter_class=argparse.ArgumentDefaultsHelpFormatter)

parser.add_argument("--size", type=int, default=256)
parser.add_argument("--output_fname", type=str, default='foo.bin')

args = parser.parse_args()

TEMPLATE = b'\xFA'

with open(args.output_fname, 'wb') as f:
	f.write(TEMPLATE * args.size)
