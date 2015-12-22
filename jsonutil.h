/* ============================================ */
/*		json-parser utility functions 			*/
/*		https://github.com/udp/json-parser		*/
/*		Tomasz Okoniewski/BlaBla - 2014			*/
/* ============================================ */
extern char *json_type_tab[];
extern int jsonlog;
extern int jsonlevel;

#ifdef __cplusplus
   extern "C"
   {
#endif

/*		OLD	FUN	*/
void rec_info(json_value *jo, int i2);
json_value *find_node(json_value *jo, char *str);
int json_get_size(json_value *jo);

/* 		AUXILIARY RETURN FUNCTION */
json_value *return_to_parent(json_value *jo);

/*		STEP TO NEIGHBORING ELEMENT	*/
json_value *get_flat_node(json_value *jo);

/*		STEP TO NEXT ELEMENT	*/
json_value *get_next_node(json_value *jo);

/*		RETURN NAME CURRENT OBJECT */
char *get_node_name(json_value *j);

/* 		RETURN CURRENT TAB POSITION	 */
int get_array_index(json_value *j);

void print_node_type(json_value *jo);

/*		LOAD AND PARSER JSON FILE	*/
json_value *loadJson(char *);

#ifdef __cplusplus
   } /* extern "C" */
#endif








