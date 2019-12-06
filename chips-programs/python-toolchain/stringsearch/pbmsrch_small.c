/* +++Date last modified: 05-Jul-1997 */

/*
**        A Pratt-Boyer-Moore string search, written by Jerry Coffin
**  sometime or other in 1991.  Removed from original program, and
**  (incorrectly) rewritten for separate, generic use in early 1992.
**  Corrected with help from Thad Smith, late March and early
**  April 1992...hopefully it's correct this time. Revised by Bob Stout.
**
**  This is hereby placed in the Public Domain by its author.
**
**  10/21/93 rdg  Fixed bug found by Jeff Dunlop
*/

#include <stddef.h>
#include <string.h>
#include <limits.h>

#define size_t unsigned

//static size_t table[UCHAR_MAX + 1];
//static size_t len;
//static char *findme;
//
///*
//**  Call this with the string to locate to initialize the table
//*/
//
//void init_search(char *string)
//{
//      size_t i;
//
//      len = strlen(string);
//      for (i = 0; i <= UCHAR_MAX; i++)                      /* rdg 10/93 */
//            table[i] = len;
//      for (i = 0; i < len; i++)
//            table[(unsigned char)string[i]] = len - i - 1;
//      findme = (char *)string;
//}
//
///*
//**  Call this with a buffer to search
//*/
//
//char *strsearch(char *string)
//{
//      register size_t shift;
//      register size_t pos = len - 1;
//      char *here;
//      size_t limit=strlen(string);
//      report(limit);
//
//      while (pos < limit)
//      {
//            while( pos < limit &&
//                  (shift = table[(unsigned char)string[pos]]) > 0)
//            {
//                  pos += shift;
//            }
//            if (0 == shift)
//            {
//                  if (0 == strncmp(findme,
//                        here = (char *)&string[pos-len+1], len))
//                  {
//                        return(here);
//                  }
//                  else  pos++;
//            }
//      }
//      return NULL;
//}

//#include <stdio.h>
int input_1 = input("input_1");
int output_1 = output("output_1");

char foo[] = "abb";
      char *find_strings[] = {foo,  NULL};
//                              "oo", "oh", "xx", "xx", "x", "x", "field", "new", "row",
//			      "regime", "boom", "that", "impact", "and", "zoom", "texture",
//			      "magnet", "doom", "loom", "freq", "current", "phase",
//			      "images", 
//			      "appears", "phase", "conductor", "wavez", 
//			      "normal", "free", "termed",
//			      "provide", "for", "and", "struct", "about", "have",
//			      "proper",
//			      "involve",
//			      "describedly",
//			      "thats",
//			      "spaces",
//			      "circumstance",
//			      "the",
//			      "member",
//			      "such",
//			      "guide",
//			      "regard",
//			      "officers",
//			      "implement",
//			      "principalities",			      
//			      NULL};

main()
{
      char *here;
      int i;
      report(find_strings[0][0]);
      report(foo[1]);
      report(foo[2]);
      report(foo[3]);
      report(strlen(find_strings[0]));
      report(foo[0]);
      report(strlen(foo));
      report(strlen("abb"));

//      for (i = 0;  ;i++)
//      {
//            if(find_strings[i] == NULL)
//              break;
//            init_search(find_strings[i]);
//            here = strsearch(search_strings[i]);
//            if(here != NULL)
//              fputc(1, output_1);
//            else
//              fputc(0, output_1);
////            printf("\"%s\" is%s in \"%s\"", find_strings[i],
////                  here ? "" : " not", search_strings[i]);
////            if (here)
////                  printf(" [\"%s\"]", here);
////            putchar('\n');
//      }

      report(42);
      return 0;
}

