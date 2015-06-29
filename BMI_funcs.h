//BMI grid function
int get_grid_origin(int grid_id, double *origin)
{
	//Origin is bottom right
	//Mark finding out more about this
}
int get_grid_shape(int grid_id, int *shape)
{
	//rows, columns in the grid shape[0], shape[1]. rows of data + 1, columns of data + 1
}
int get_grid_spacing(int grid_id, double *spacing)
{
	//spacing in x, y directions spacing[0], spacing[1]
}
int get_grid_rank(int grid_id, int *rank)
{
	//depending on grid
	*rank = 2;
}
int get_grid_type(int grid_id, char *type)
{
	//"uniform_recitilinear"
}
int get_var_units(char *name, char *units)
{
	if (name == "surface__elevation")
	{
		units = "meters";
	}
}
int get_var_type(char *name, int *nbytes)
{
	if (name == "surface__elevation")
	{
		type = "double";
	}
}
int get_var_nbytes(char *name, char *units)
{
	if (name == "surface__elevation")
	{
		//memory required by the entire grid
		*units = ;//size of units * size of grid
	}
}
int get_var_itemsize(char *name, int *itemsize)
{
	if (name == "surface__elevation")
	{
		*itemsize = sizeof(double);
	}
}
int get_var_grid(char *name, int *grid)
{
	if (name == "surface__elevation")
	{
		*grid = 0;
	}
}
int get_value(char *name, void *buffer)
{
	int status;
	int nbytes = get_var_nbytes(name, &nbytes)
	
	void *src = NULL;
	
	status = get_value_ref(name, &src);
	
	//in C memcpy(buffer, src, nbytes);
}
int set_value(char *name, void *buffer)
{
	int status;
	void *dest = NULL;
	int nbytes = 0;
	
	stauts = get_value_ref(name, &dest);
	
	status = get_var_nbytes(name, &nbytes)
	
	//copy buffer to dest
}
int set_value_at_indices(char *name, int *indices, int len, void *buffer)
{
	//take values from buffer and copy to corresponding indices in elevation array
}
int get_value_ref(char *name, void **buffer)
{
	void *src = NULL;
	
	if (name == "surface__elevation")
	{
		src = ;//pointer to elevation data in array
	}
	
	*buffer = src;
}
int get_value_at_indicies(char *name, void *buffer, int *indices, int len)
{
	void *src;
	int status, itemsize;
	get_var_itemsize(name, &itemsize)
	
	status = get_value_ref(name, &src);
	
	//len = # of elements in indices
	//fill buffer with elevation values that corresponded to the int values in indices
}