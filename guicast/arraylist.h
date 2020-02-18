
/*
 * CINELERRA
 * Copyright (C) 2008 Adam Williams <broadcast at earthling dot net>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * 
 */

#ifndef ARRAYLIST_H
#define ARRAYLIST_H

// designed for lists of track numbers

#include <stdio.h>
#include <stdlib.h>

#define ARRAYLIST_REMOVEOBJECT_DELETE 0
#define ARRAYLIST_REMOVEOBJECT_DELETEARRAY 1
#define ARRAYLIST_REMOVEOBJECT_FREE 2

template<class TYPE>
class ArrayList
{
public:
	ArrayList();
	virtual ~ArrayList();

	TYPE append(TYPE value);
	TYPE append();
	TYPE insert(TYPE value, int number);

// allocate
	void allocate(int total);
// remove last pointer from end
	void remove();          
// remove last pointer and object from end
	void remove_object();          
// remove pointer to object from list
	void remove(TYPE value);       
// remove object and pointer to it from list
	void remove_object(TYPE value);     
// remove object and pointer to it from list
	void remove_object_number(int number);     
// remove pointer to item numbered
	void remove_number(int number);
// Return number of first object matching argument
	int number_of(TYPE object);
	void remove_all();
// Remove pointer and objects for each array entry
	void remove_all_objects();
// Get last element in list
	TYPE last();
// Call this if the TYPE is a pointer to an array which must be
// deleted by delete [].
	void set_array_delete();
	void set_free();

	void sort();

	TYPE* values;
	int total;

private:
	int available;
	int removeobject_type;
};

template<class TYPE>
ArrayList<TYPE>::ArrayList()
{
	total = 0;
	available = 16;
	removeobject_type = ARRAYLIST_REMOVEOBJECT_DELETE;
	values = new TYPE[available];
}


template<class TYPE>
ArrayList<TYPE>::~ArrayList()
{
// Just remove the pointer
	delete [] values;
	values = 0;
}

template<class TYPE>
void ArrayList<TYPE>::set_array_delete()
{
    removeobject_type = ARRAYLIST_REMOVEOBJECT_DELETEARRAY;

}

template<class TYPE>
void ArrayList<TYPE>::set_free()
{
    removeobject_type = ARRAYLIST_REMOVEOBJECT_FREE;

}


template<class TYPE>
void ArrayList<TYPE>::allocate(int total)
{
	if(total > available)
	{
		available = total;
		TYPE* newvalues = new TYPE[available];
		for(int i = 0; i < total; i++) newvalues[i] = values[i];
		delete [] values;
		values = newvalues;
	}
}

template<class TYPE>
TYPE ArrayList<TYPE>::append(TYPE value)            // add to end of list
{
	if(total + 1 > available) 
	{
		available *= 2;
		TYPE* newvalues = new TYPE[available];
		for(int i = 0; i < total; i++) newvalues[i] = values[i];
		delete [] values;
		values = newvalues;
	}
	
	values[total++] = value;
	return value;
}

template<class TYPE>
TYPE ArrayList<TYPE>::append()            // add to end of list
{
	if(total + 1 > available)
	{
		available *= 2;
		TYPE* newvalues = new TYPE[available];
		for(int i = 0; i < total; i++) newvalues[i] = values[i];
		delete [] values;
		values = newvalues;
	}
	total++;

	return values[total - 1];
}

template<class TYPE>
TYPE ArrayList<TYPE>::insert(TYPE value, int number)
{
	append(0);
	for(int i = total - 1; i > number; i--)
	{
		values[i] = values[i - 1];
	}
	values[number] = value;
}

template<class TYPE>
void ArrayList<TYPE>::remove(TYPE value)                   // remove value from anywhere in list
{
	int in, out;

	for(in = 0, out = 0; in < total;)
	{
		if(values[in] != value) values[out++] = values[in++];
		else 
		{
			in++; 
		}
	}
	total = out;
}

template<class TYPE>
TYPE ArrayList<TYPE>::last()                   // last element in list
{
	return values[total - 1];
}



template<class TYPE>
void ArrayList<TYPE>::remove_object(TYPE value)                   // remove value from anywhere in list
{
	remove(value);
	switch (removeobject_type)
	{
		case ARRAYLIST_REMOVEOBJECT_DELETE:
			delete value;
			break;
		case ARRAYLIST_REMOVEOBJECT_DELETEARRAY:
			delete [] value;
			break;
		case ARRAYLIST_REMOVEOBJECT_FREE:
			free(value);
			break;
		default:
			printf("Unknown function to use to free array\n");
			break;
	}
}

template<class TYPE>
void ArrayList<TYPE>::remove_object_number(int number)
{
	if(number < total)
	{
		switch (removeobject_type)
		{
			case ARRAYLIST_REMOVEOBJECT_DELETE:
				delete values[number];
				break;
			case ARRAYLIST_REMOVEOBJECT_DELETEARRAY:
				delete [] values[number];
				break;
			case ARRAYLIST_REMOVEOBJECT_FREE:
				free(values[number]);
				break;
			default:
				printf("Unknown function to use to free array\n");
				break;
		}

		remove_number(number);
	}
	else
		fprintf(stderr, "ArrayList<TYPE>::remove_object_number: number %d out of range %d.\n", number, total);
}


template<class TYPE>
void ArrayList<TYPE>::remove_object()                   // remove last object from array
{
	if(total)
	{
		switch (removeobject_type)
		{
			case ARRAYLIST_REMOVEOBJECT_DELETE:
				delete values[total - 1];
				break;
			case ARRAYLIST_REMOVEOBJECT_DELETEARRAY:
				delete [] values[total -1];
				break;
			case ARRAYLIST_REMOVEOBJECT_FREE:
				free(values[total - 1]);
				break;
			default:
				printf("Unknown function to use to free array\n");
				break;
		}

		remove();
	}
	else
		fprintf(stderr, "ArrayList<TYPE>::remove_object: array is 0 length.\n");
}



template<class TYPE>
void ArrayList<TYPE>::remove()
{
	total--;
}

// remove pointer from anywhere in list
template<class TYPE>
void ArrayList<TYPE>::remove_number(int number)                   
{
	int in, out;
	for(in = 0, out = 0; in < total;)
	{
		if(in != number)
			values[out++] = values[in++];
		else
// need to delete it here
			in++;       
	}
	total = out;
}

template<class TYPE>
void ArrayList<TYPE>::remove_all_objects()
{
//printf("ArrayList<TYPE>::remove_all_objects 1 %d\n", total);
	for(int i = 0; i < total; i++)
	{
		switch (removeobject_type)
		{
			case ARRAYLIST_REMOVEOBJECT_DELETE:
				delete values[i];
				break;
			case ARRAYLIST_REMOVEOBJECT_DELETEARRAY:
				delete [] values[i];
				break;
			case ARRAYLIST_REMOVEOBJECT_FREE:
				free(values[i]);
				break;
			default:
				printf("Unknown function to use to free array\n");
				break;
		}
	}
	total = 0;
}

template<class TYPE>
void ArrayList<TYPE>::remove_all()
{
	total = 0;
}

// sort from least to greatest value
template<class TYPE>
void ArrayList<TYPE>::sort()
{
	int result = 1;
	TYPE temp;

	while(result)
	{
		result = 0;
		for(int i = 0, j = 1; j < total; i++, j++)
		{
			if(values[j] < values[i])
			{
				temp = values[i];
				values[i] = values[j];
				values[j] = temp;
				result = 1;
			}
		}
	}
}

template<class TYPE>
int ArrayList<TYPE>::number_of(TYPE object)
{
	for(int i = 0; i < total; i++)
	{
		if(values[i] == object) return i;
	}
	return 0;
}


#endif
