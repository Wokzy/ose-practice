
import random
import argparse

parser = argparse.ArgumentParser(formatter_class=argparse.ArgumentDefaultsHelpFormatter)

parser.add_argument("--n-sectors", type=int, default=8)
parser.add_argument("--sector-size", type=int, default=512)
parser.add_argument("--output_fname", type=str, default='zero.bin')

args = parser.parse_args()

with open(args.output_fname, 'wb') as f:
	# f.write(random.randbytes(args.sector_size * args.n_sectors))
	f.write(b'\x00' * (args.sector_size * args.n_sectors))
