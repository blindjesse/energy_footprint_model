/* Structures used in the customized binary files.
*/
typedef struct STOREC{ /* these are used in the fread statements for temporary storeage */ 
	int			row;
	int			col;
	int			value;
}STOREC;
STOREC		*SCptr;


typedef struct HEAD{
	long long	counter;
	float		grain;
	int			row;
	int			col;
	float		ULE;  /* upper left UTME */
	float		ULN;  /* upper left UTMN */
}HEAD;
HEAD		*HEptr;