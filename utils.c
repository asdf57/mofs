int
bitflag_to_idx(int val) {
	int i;

	for (i = 0; val; val >>= 1, i++)
		if (val & 1)
			return i;

	return -1;
}