#include <cstdlib>
#include <iostream>
#include <fstream>
#include <stdio.h>

#include "json.h"
#include "jsonutil.h"
#include "utfjson.h"

using namespace std;
/*
void rec_info(json_value *jo, int i2);
json_value *find_node(json_value *jo, char *str);
json_value *return_to_parent(json_value *jo);
int json_pan_foto_licznik=0;
void print_node_type(json_value *jo);
*/
int jsonlog=0;
int jsonlevel=0;

char *json_type_tab[]={	 
	"json_none",
   "json_object",
   "json_array",
   "json_integer",
   "json_double",
   "json_string",
   "json_boolean",
   "json_null"};

json_value *loadJson(char *fname)
{

char *test2;
json_value *jo=0;
 
 //ifstream is("get_panoramas.js"); //, std::ifstream::binary);
 ifstream is(fname);
  if (is) {
    // get length of file:
    is.seekg (0, is.end);
    int length = is.tellg();
    is.seekg (0, is.beg);

    char * buffer = new char [length];
    
    if (jsonlog) std::cout << "Reading " << length << " characters... " << endl;
    if (jsonlog) std::cout << "Address: " << &buffer << endl;
    // read data as a block:
    is.read (buffer,length);
    
    json_utf16hack(buffer,length, 'z');  //json uft16 magic preparser :)
    
    jo=json_parse(buffer, length);    
    
    json_utf16revert(jo);
    
    //if (jo) rec_info(jo, 1);    
    if (jo)
    {
	/*cout << jo->type << endl;*/
	if (jsonlog) cout << jo->parent << endl;

	//rec_info(jo, 0);
	find_node(jo, "photo_title");
    
	// load !!!!!! przy parse odremowac
    //json_value_free(jo);
    }
    else
        cout << "parser error" << endl;
	delete buffer;
   }
   else
   {
    cout << "file error" << endl;
   }
	return jo;
}

int get_array_index(json_value *j)
{
	int k=0;
	if (j->parent->type==json_array)
	{
		while (j->parent->u.array.values[k]!=j)
			k++;
			return ++k;
	}
	else return 0;
}

char *get_node_name(json_value *j)
{
	int k=0;
	json_value *j2=0;
	static char noname[] = "no name";
	static char object[] = "object";
	static char nonode[] = "no_node";
	static char endarray[] = "array end";
	static char cell[] = "%s[%d]";
	static char buf[24];


	j2=j->parent;
	if (!j)
		return nonode;
	if (!j2)
		return nonode;
	else
	{
		if (j2->type==json_array)
		{
			while (j2->u.array.values[k]!=j)
				k++;
				sprintf(buf, cell, get_node_name(j2), k);
			//cout << get_node_name(j2) << " : " << k << endl;				
				return buf;
		}
		if (j2->type==json_object)
		{
			while (j2->u.object.values[k].value!=j)
				k++;
				if (k<j2->u.object.length)
					return j2->u.object.values[k].name;
				else
					return endarray;
		}
	}
	return noname;
}

json_value *get_flat_node(json_value *jo)
{
	int k;
     json_value *nn=0;
     json_value *parent=jo->parent;
     if (!parent)
        return nn;
     if (!jo)
        return nn;
	switch (parent->type)
	{
		case json_object :
			for (k=0; k<parent->u.object.length && parent->u.object.values[k].value!=jo; k++)
				if (jsonlog) printf(" %s \n", parent->u.object.values[k].name);
				k++;
				if (k<parent->u.object.length)
					nn=parent->u.object.values[k].value;
				else
					jsonlevel--;
             break;				
      case json_array :
           for (k=0; k<parent->u.array.length && parent->u.array.values[k]!=jo; k++)
				if (jsonlog) printf(".");
				k++;
				if (k<parent->u.array.length)
					nn=parent->u.array.values[k];
				else
					jsonlevel--;					
				if (jsonlog) printf(" %d\n", k);
			//jsonlevel--;
          break;
      }
	  //printf(" Zwracam 0 z obiektu/tablicy\n");
      return nn;
}

json_value *return_to_parent(json_value *jo)
{
	int k;
     json_value *nn=0;
     json_value *parent=jo->parent;
     if (!parent)
        return nn;
     if (!jo)
        return nn;
	 switch (parent->type)
	 {
	   case json_object :
			//printf(" %s \n", j->u.object.values[0].name);
			if (jsonlog)
           printf(" retrun 2 object from %X\n", jo);
           /*for (k=0; k<parent->u.object.length; k++)
          	{
               printf(" %x %x %x\n", &(parent->u.object.values[k]), parent->u.object.values[k], parent->u.object.values[k].value);
          	}*/
           for (k=0; k<parent->u.object.length && parent->u.object.values[k].value!=jo; k++)
	   	            if (jsonlog) printf(".");
				if (jsonlog) printf(" %s \n", parent->u.object.values[k].name);
				k++;
				if (k<parent->u.object.length)
					nn=parent->u.object.values[k].value;
				else
				{
					jsonlevel--;
                    nn=return_to_parent(parent);
				}
				if (jsonlog) printf(" %d\n", k);
			//jsonlevel--;
             break;
       case json_array :
			if (jsonlog)
           printf(" retrun 2 array from %X\n", jo);
           /*for (k=0; k<parent->u.array.length; k++)
          	{
               printf(" %x %x\n", &(parent->u.array.values[k]), parent->u.array.values[k]);
          	}*/
           for (k=0; k<parent->u.array.length && parent->u.array.values[k]!=jo; k++)
	   	            if (jsonlog) printf(".");
				k++;
				if (k<parent->u.array.length)
					nn=parent->u.array.values[k];
				else
                {
					jsonlevel--;
                    nn=return_to_parent(parent);
				}
				if (jsonlog) printf(" %d\n", k);
			//jsonlevel--;
          break;
      }
	  //printf(" Zwracam 0 z obiektu/tablicy\n");
      return nn;           
}
void print_node_type(json_value *jo)
{
     json_value *j=jo;
     if (!jo)
        return;
        printf("========= ");
	 switch (j->type)
	 {
	   case json_object :
			//if (j->parent!=0)
			//	printf(" %s \n", j->parent->u.object.values[0].name);
			//printf(" %s \n", j->u.object.values[0].name);
			printf("OBJECT \n");
          break;
                  
       case json_array :
			cout << "ARRAY" << endl;
			//printf(" %s \n", j->u.array.values[0].name);
          break;
          
       case json_integer :
            //printf(" %s ", return_to_parent(j)->u.object.values[0].name)
            printf(" INT %s\t %d \n", get_node_name(j), j->u.integer);
          break;
           
       case json_double :
            printf(" DBL %s\t %f \n", get_node_name(j), j->u.dbl);
          break;
          
       case json_string :
            printf(" STR %s\t %s \n", get_node_name(j), j->u.string.ptr);
          break;
          
       case json_null :
            printf(" NULL %s\t %s \n",  get_node_name(j), "null");
          break;
       case json_none :
            printf(" NONE %s\t %s \n",  get_node_name(j), "no name");
          break;
       case json_boolean :
            printf(" BOOL %s\t %s \n",  get_node_name(j), j->u.boolean==1?"True":"False");
          break;
		};   
     return;
}
int json_get_size(json_value *jo)
{
	switch (jo->type)
	{
	   case json_object :
		return jo->u.object.length;
		break;
       case json_array :
		return jo->u.array.length;
		break;
	}
	return 0;
}

json_value *get_next_node(json_value *jo)
{
     json_value *nn=0;
     json_value *j=jo;
     if (!jo)
        return nn;
        if (jsonlog) printf(" %X %X ", j, j->parent);
	 switch (j->type)
	 {
	   case json_object :
			//if (j->parent!=0)
			//	printf(" %s \n", j->parent->u.object.values[0].name);
			if (j->u.object.length==0)
			{
				//nn=return_to_parent(j);
				nn=get_flat_node(j);
				break;
			}
			if (jsonlog) printf(" %s \n", j->u.object.values[0].name);
			nn=j->u.object.values[0].value;
			jsonlevel++;
          break;
                  
       case json_array :
			if (j->u.array.length==0)
			{
				//cout << "array 0!\n";	
				nn=get_flat_node(j);
				break;
			}
			if (jsonlog) cout << "ARRAY" << endl;
			//printf(" %s \n", j->u.array.values[0].name);
			nn=j->u.array.values[0];
			jsonlevel++;
          break;
          
       case json_integer :
            if (jsonlog) printf(" INT %d \n", j->u.integer);
            nn=return_to_parent(j);
          break;
           
       case json_double :
            if (jsonlog) printf(" DBL %f \n", j->u.dbl);
            nn=return_to_parent(j);
          break;
          
       case json_string :
            if (jsonlog) printf(" STR %s \n", j->u.string.ptr);
            nn=return_to_parent(j);
          break;
		  
       case json_none :
            if (jsonlog) printf(" NON %s \n", "none");
            nn=return_to_parent(j);
          break;          
		  
       case json_null :
            if (jsonlog) printf(" NUL %s \n", "null");
            nn=return_to_parent(j);
          break;		  
		  
       case json_boolean :
            if (jsonlog) printf(" BOOL %s \n", j->u.boolean==1?"True":"False");
            nn=return_to_parent(j);
          break;
          };   
     return nn;
}

json_value *find_node(json_value *jo, char *str)
{
 	 int i;
 	 json_value *j=jo;
 	 json_value *retjo=0;
	 if (jo=0)
	 return 0;
	 //cout << "=======" << endl;
 	 //cout << "Typ obiektu: " << json_type_tab[j->type] << " ";// << endl;

	 switch (j->type)
	 {
	 case json_object :
	  	 //==cout << "Dlugosc obiektu: "  << j->u.object.length << endl;
	  	 for(i=0; i<j->u.object.length; i++)
		 {
			//cout << "    " << i << " node: \"" << j->u.object.values[i].name << "\" ";// << endl;	 		  
			if (strncmp(j->u.object.values[i].name, str, strlen(str))==0)
			{
				retjo=j->u.object.values[i].value;
                return retjo;
				//cout << " TRAFIONY!" << endl;
				if (j->u.object.values[i].value->type==json_string)
					cout << ": " << j->u.object.values[i].value->u.string.ptr << endl;
			}
			else
			find_node(j->u.object.values[i].value, str);			
		 }
		  break;
	 case json_string :
			//cout << "\"" << j->u.string.ptr << "\"" << endl;
		  break;
	 case json_integer :	
	 	  //cout << "\"" << j->u.integer << "\"" << endl;
		  break;
	 case json_double :	
	 	  //cout << "\"" << j->u.dbl << "\"" << endl;
		  break;
	 case json_boolean :	
	 	  //cout << "\"" << j->u.boolean << "\"" << endl;
		  break;
	 case json_array :
	  	 //==cout << "Dlugosc tablicy: "  << j->u.array.length << endl;
	  	 for(i=0; i<j->u.array.length; i++)
		 {
	 	 //cout << "\t" << i << " element: \"" << j->u.array.values[i] << "\" ";// << endl;	 		  
			find_node(j->u.array.values[i], str);
		 }
		  break;
	 case json_null :	
		  break;
		  
		  default :	
		  return 0;
	 return retjo;
	};
}

void rec_info(json_value *jo, int i2)
{
 	 int i,c;
 	 json_value *j=jo;
 	 if (jo=0)
	 return;
	 //cout << "=======" << endl;
 	 cout << "Typ obiektu: " << json_type_tab[j->type] << " ";// << endl;

	 switch (j->type)
	 {
	 case json_object :
	  	 cout << "Dlugosc obiektu: "  << j->u.object.length << endl;
	  	 for(i=0; i<j->u.object.length; i++)
		 {
	 	 cout << "    " << i << " node: \"" << j->u.object.values[i].name << "\" ";// << endl;	 		  
		    //cout << j->u.object.values[i].value << endl;
			//if (strncmp(j->u.object.values[i].name,"photo_",5)==0)
				rec_info(j->u.object.values[i].value, i);
			//else
				//cout << endl;				
		 }
		  break;
	 case json_string :
          if (strncmp((j->parent)->u.object.values[i2].name,"photo_title",10)==0)
			//cout << (j->parent)->u.object.values[i2].name;
			cout << "\"" << j->u.string.ptr << "\"" << endl;
			else
			cout << endl;
		  break;
	 case json_integer :	
	 	  cout << "\"" << j->u.integer << "\"" << endl;
		  break;
	 case json_double :	
	 	  cout << "\"" << j->u.dbl << "\"" << endl;
		  break;
	 case json_boolean :	
	 	  cout << "\"" << j->u.boolean << "\"" << endl;
		  break;
	 case json_array :
	  	 cout << "Dlugosc tablicy: "  << j->u.array.length << endl;
		 c=j->u.array.length;
		 if (c>5)
			c=5;
	  	 for(i=0; i<c; i++)
		 {
	 	 cout << "\t" << i << " element: \"" << j->u.array.values[i] << "\" ";// << endl;	 		  
		    //cout << j->u.object.values[i].value << endl;
			rec_info(j->u.array.values[i], i);
		 }
		  break;
	 case json_null :	
	 	  cout << " \"NULL\"  " << endl;
		  break;
		  default :
		  cout << "Not recognize element" << endl;		
	};
}
											
											
											
											

																						
											

																						
											

																						
											
