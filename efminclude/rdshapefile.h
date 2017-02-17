typedef struct STORESH {
	char	active; /*1= active, -1 is inactive */
	int		time;   /* time when a road became inactive */
}STORESH;
STORESH		*storerSH;

int		roadbSH,roadsSH;
int		MAXRDENDSH;