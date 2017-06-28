/*
** Compile and run this standalone program in order to generate code that
** implements a function that will translate alphabetic identifiers into
** parser token codes.
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

/*
** A header comment placed at the beginning of generated code.
*/
static const char zHdr[] = 
  "/***** This file contains automatically generated code ******\n"
  "**\n"
  "** The code in this file has been automatically generated by\n"
  "**\n"
  "**   sqlite/tool/mkkeywordhash.c\n"
  "**\n"
  "** The code in this file implements a function that determines whether\n"
  "** or not a given identifier is really an SQL keyword.  The same thing\n"
  "** might be implemented more directly using a hand-written hash table.\n"
  "** But by using this automatically generated code, the size of the code\n"
  "** is substantially reduced.  This is important for embedded applications\n"
  "** on platforms with limited memory.\n"
  "*/\n"
;

/*
** All the keywords of the SQL language are stored in a hash
** table composed of instances of the following structure.
*/
typedef struct Keyword Keyword;
struct Keyword {
  char *zName;         /* The keyword name */
  char *zTokenType;    /* Token value for this keyword */
  int mask;            /* Code this keyword if non-zero */
  int reserved;        /* If non-zero this word cannot be used as identifier */
  int id;              /* Unique ID for this record */
  int hash;            /* Hash on the keyword */
  int offset;          /* Offset to start of name string */
  int len;             /* Length of this keyword, not counting final \000 */
  int prefix;          /* Number of characters in prefix */
  int longestSuffix;   /* Longest suffix that is a prefix on another word */
  int iNext;           /* Index in aKeywordTable[] of next with same hash */
  int substrId;        /* Id to another keyword this keyword is embedded in */
  int substrOffset;    /* Offset into substrId for start of this keyword */
  char zOrigName[20];  /* Original keyword name before processing */
};

/*
** Define masks used to determine which keywords are allowed
*/
#ifdef SQLITE_OMIT_ALTERTABLE
#  define ALTER      0
#else
#  define ALTER      0x00000001
#endif
#define ALWAYS       0x00000002
#ifdef SQLITE_OMIT_ANALYZE
#  define ANALYZE    0
#else
#  define ANALYZE    0x00000004
#endif
#ifdef SQLITE_OMIT_ATTACH
#  define ATTACH     0
#else
#  define ATTACH     0x00000008
#endif
#ifdef SQLITE_OMIT_AUTOINCREMENT
#  define AUTOINCR   0
#else
#  define AUTOINCR   0x00000010
#endif
#ifdef SQLITE_OMIT_CAST
#  define CAST       0
#else
#  define CAST       0x00000020
#endif
#ifdef SQLITE_OMIT_COMPOUND_SELECT
#  define COMPOUND   0
#else
#  define COMPOUND   0x00000040
#endif
#ifdef SQLITE_OMIT_CONFLICT_CLAUSE
#  define CONFLICT   0
#else
#  define CONFLICT   0x00000080
#endif
#ifdef SQLITE_OMIT_EXPLAIN
#  define EXPLAIN    0
#else
#  define EXPLAIN    0x00000100
#endif
#ifdef SQLITE_OMIT_FOREIGN_KEY
#  define FKEY       0
#else
#  define FKEY       0x00000200
#endif
#ifdef SQLITE_OMIT_PRAGMA
#  define PRAGMA     0
#else
#  define PRAGMA     0x00000400
#endif
#ifdef SQLITE_OMIT_REINDEX
#  define REINDEX    0
#else
#  define REINDEX    0x00000800
#endif
#ifdef SQLITE_OMIT_SUBQUERY
#  define SUBQUERY   0
#else
#  define SUBQUERY   0x00001000
#endif

#define TRIGGER    0x00002000

#if defined(SQLITE_OMIT_AUTOVACUUM) && \
    (defined(SQLITE_OMIT_VACUUM) || defined(SQLITE_OMIT_ATTACH))
#  define VACUUM     0
#else
#  define VACUUM     0x00004000
#endif
#ifdef SQLITE_OMIT_VIEW
#  define VIEW       0
#else
#  define VIEW       0x00008000
#endif
#ifdef SQLITE_OMIT_VIRTUALTABLE
#  define VTAB       0
#else
#  define VTAB       0x00010000
#endif
#ifdef SQLITE_OMIT_AUTOVACUUM
#  define AUTOVACUUM 0
#else
#  define AUTOVACUUM 0x00020000
#endif
#ifdef SQLITE_OMIT_CTE
#  define CTE        0
#else
#  define CTE        0x00040000
#endif

/*
** These are the keywords
*/
static Keyword aKeywordTable[] = {
  { "ABORT",            "TK_ABORT",        CONFLICT|TRIGGER,       1},
  { "ACTION",           "TK_ACTION",       FKEY,                   0},
  { "ADD",              "TK_ADD",          ALTER,                  0},
  { "AFTER",            "TK_AFTER",        TRIGGER,                0},
  { "ALL",              "TK_ALL",          ALWAYS,                 1},
  { "ALTER",            "TK_ALTER",        ALWAYS,                 1},
  { "ANALYZE",          "TK_ANALYZE",      ALWAYS,                 1},
  { "AND",              "TK_AND",          ALWAYS,                 1},
  { "AS",               "TK_AS",           ALWAYS,                 1},
  { "ASC",              "TK_ASC",          ALWAYS,                 1},
  { "ATTACH",           "TK_ATTACH",       ATTACH,                 1},
  { "AUTOINCREMENT",    "TK_AUTOINCR",     AUTOINCR,               0},
  { "BEFORE",           "TK_BEFORE",       TRIGGER,                0},
  { "BEGIN",            "TK_BEGIN",        ALWAYS,                 1},
  { "BETWEEN",          "TK_BETWEEN",      ALWAYS,                 1},
  { "BY",               "TK_BY",           ALWAYS,                 1},
  { "CASCADE",          "TK_CASCADE",      FKEY,                   0},
  { "CASE",             "TK_CASE",         ALWAYS,                 1},
  { "CAST",             "TK_CAST",         CAST,                   1},
  { "CHECK",            "TK_CHECK",        ALWAYS,                 1},
  { "COLLATE",          "TK_COLLATE",      ALWAYS,                 1},
  { "COLUMN",           "TK_COLUMNKW",     ALTER,                  0},
  { "COMMIT",           "TK_COMMIT",       ALWAYS,                 1},
  { "CONFLICT",         "TK_CONFLICT",     CONFLICT,               1},
  { "CONSTRAINT",       "TK_CONSTRAINT",   ALWAYS,                 1},
  { "CREATE",           "TK_CREATE",       ALWAYS,                 1},
  { "CROSS",            "TK_JOIN_KW",      ALWAYS,                 1},
  { "CURRENT_DATE",     "TK_CTIME_KW",     ALWAYS,                 1},
  { "CURRENT_TIME",     "TK_CTIME_KW",     ALWAYS,                 1},
  { "CURRENT_TIMESTAMP","TK_CTIME_KW",     ALWAYS,                 1},
  { "DATABASE",         "TK_DATABASE",     ATTACH,                 1},
  { "DEFAULT",          "TK_DEFAULT",      ALWAYS,                 1},
  { "DEFERRED",         "TK_DEFERRED",     ALWAYS,                 1},
  { "DEFERRABLE",       "TK_DEFERRABLE",   FKEY,                   0},
  { "DELETE",           "TK_DELETE",       ALWAYS,                 1},
  { "DESC",             "TK_DESC",         ALWAYS,                 1},
  { "DETACH",           "TK_DETACH",       ATTACH,                 1},
  { "DISTINCT",         "TK_DISTINCT",     ALWAYS,                 1},
  { "DROP",             "TK_DROP",         ALWAYS,                 1},
  { "END",              "TK_END",          ALWAYS,                 1},
  { "EACH",             "TK_EACH",         TRIGGER,                0},
  { "ELSE",             "TK_ELSE",         ALWAYS,                 1},
  { "ESCAPE",           "TK_ESCAPE",       ALWAYS,                 1},
  { "EXCEPT",           "TK_EXCEPT",       COMPOUND,               1},
  { "EXCLUSIVE",        "TK_EXCLUSIVE",    ALWAYS,                 1},
  { "EXISTS",           "TK_EXISTS",       ALWAYS,                 1},
  { "EXPLAIN",          "TK_EXPLAIN",      EXPLAIN,                1},
  { "FAIL",             "TK_FAIL",         CONFLICT|TRIGGER,       1},
  { "FOR",              "TK_FOR",          ALWAYS,                 0},
  { "FOREIGN",          "TK_FOREIGN",      FKEY,                   0},
  { "FROM",             "TK_FROM",         ALWAYS,                 1},
  { "FULL",             "TK_JOIN_KW",      ALWAYS,                 1},
  { "GLOB",             "TK_LIKE_KW",      ALWAYS,                 1},
  { "GROUP",            "TK_GROUP",        ALWAYS,                 1},
  { "HAVING",           "TK_HAVING",       ALWAYS,                 1},
  { "IF",               "TK_IF",           ALWAYS,                 1},
  { "IGNORE",           "TK_IGNORE",       CONFLICT|TRIGGER,       1},
  { "IMMEDIATE",        "TK_IMMEDIATE",    ALWAYS,                 1},
  { "IN",               "TK_IN",           ALWAYS,                 1},
  { "INDEX",            "TK_INDEX",        ALWAYS,                 1},
  { "INDEXED",          "TK_INDEXED",      ALWAYS,                 1},
  { "INITIALLY",        "TK_INITIALLY",    FKEY,                   0},
  { "INNER",            "TK_JOIN_KW",      ALWAYS,                 1},
  { "INSERT",           "TK_INSERT",       ALWAYS,                 1},
  { "INSTEAD",          "TK_INSTEAD",      TRIGGER,                0},
  { "INTERSECT",        "TK_INTERSECT",    COMPOUND,               1},
  { "INTO",             "TK_INTO",         ALWAYS,                 1},
  { "IS",               "TK_IS",           ALWAYS,                 1},
  { "ISNULL",           "TK_ISNULL",       ALWAYS,                 1},
  { "JOIN",             "TK_JOIN",         ALWAYS,                 1},
  { "KEY",              "TK_KEY",          ALWAYS,                 1},
  { "LEFT",             "TK_JOIN_KW",      ALWAYS,                 1},
  { "LIKE",             "TK_LIKE_KW",      ALWAYS,                 1},
  { "LIMIT",            "TK_LIMIT",        ALWAYS,                 1},
  { "MATCH",            "TK_MATCH",        ALWAYS,                 1},
  { "NATURAL",          "TK_JOIN_KW",      ALWAYS,                 1},
  { "NO",               "TK_NO",           ALWAYS,                 0},
  { "NOT",              "TK_NOT",          ALWAYS,                 1},
  { "NOTNULL",          "TK_NOTNULL",      ALWAYS,                 1},
  { "NULL",             "TK_NULL",         ALWAYS,                 1},
  { "OF",               "TK_OF",           ALWAYS,                 0},
  { "OFFSET",           "TK_OFFSET",       ALWAYS,                 1},
  { "ON",               "TK_ON",           ALWAYS,                 1},
  { "OR",               "TK_OR",           ALWAYS,                 1},
  { "ORDER",            "TK_ORDER",        ALWAYS,                 1},
  { "OUTER",            "TK_JOIN_KW",      ALWAYS,                 1},
  { "PLAN",             "TK_PLAN",         EXPLAIN,                1},
  { "PRAGMA",           "TK_PRAGMA",       PRAGMA,                 1},
  { "PRIMARY",          "TK_PRIMARY",      ALWAYS,                 1},
  { "QUERY",            "TK_QUERY",        EXPLAIN,                1},
  { "RAISE",            "TK_RAISE",        TRIGGER,                0},
  { "RECURSIVE",        "TK_RECURSIVE",    CTE,                    1},
  { "REFERENCES",       "TK_REFERENCES",   FKEY,                   0},
  { "REGEXP",           "TK_LIKE_KW",      ALWAYS,                 1},
  { "REINDEX",          "TK_REINDEX",      REINDEX,                1},
  { "RELEASE",          "TK_RELEASE",      ALWAYS,                 1},
  { "RENAME",           "TK_RENAME",       ALTER,                  0},
  { "REPLACE",          "TK_REPLACE",      CONFLICT,               1},
  { "RESTRICT",         "TK_RESTRICT",     FKEY,                   0},
  { "RIGHT",            "TK_JOIN_KW",      ALWAYS,                 1},
  { "ROLLBACK",         "TK_ROLLBACK",     ALWAYS,                 1},
  { "ROW",              "TK_ROW",          TRIGGER,                0},
  { "SAVEPOINT",        "TK_SAVEPOINT",    ALWAYS,                 1},
  { "SELECT",           "TK_SELECT",       ALWAYS,                 1},
  { "SET",              "TK_SET",          ALWAYS,                 1},
  { "TABLE",            "TK_TABLE",        ALWAYS,                 1},
  { "TEMP",             "TK_TEMP",         ALWAYS,                 1},
  { "TEMPORARY",        "TK_TEMP",         ALWAYS,                 1},
  { "THEN",             "TK_THEN",         ALWAYS,                 1},
  { "TO",               "TK_TO",           ALWAYS,                 1},
  { "TRANSACTION",      "TK_TRANSACTION",  ALWAYS,                 1},
  { "TRIGGER",          "TK_TRIGGER",      TRIGGER,                0},
  { "UNION",            "TK_UNION",        COMPOUND,               1},
  { "UNIQUE",           "TK_UNIQUE",       ALWAYS,                 1},
  { "UPDATE",           "TK_UPDATE",       ALWAYS,                 1},
  { "USING",            "TK_USING",        ALWAYS,                 1},
  { "VACUUM",           "TK_VACUUM",       VACUUM,                 1},
  { "VALUES",           "TK_VALUES",       ALWAYS,                 1},
  { "VIEW",             "TK_VIEW",         VIEW,                   0},
  { "VIRTUAL",          "TK_VIRTUAL",      VTAB,                   0},
  { "WITH",             "TK_WITH",         CTE,                    1},
  { "WITHOUT",          "TK_WITHOUT",      ALWAYS,                 1},
  { "WHEN",             "TK_WHEN",         ALWAYS,                 1},
  { "WHERE",            "TK_WHERE",        ALWAYS,                 1},


/**** COMDB2 CUSTOM ***********************************************/
  { "AGGREGATE",        "TK_AGGREGATE",     ALWAYS,                 0},
  { "ALIAS",            "TK_ALIAS",         ALWAYS,                 0},
  { "ANALYZESQLITE",    "TK_ANALYZESQLITE", ALWAYS,                 1},
  { "AUTHENTICATION",   "TK_AUTHENTICATION",ALWAYS,                 0},
  { "BLOBFIELD",        "TK_BLOBFIELD",     ALWAYS,                 0},
  { "BULKIMPORT",       "TK_BULKIMPORT",    ALWAYS,                 0},
  { "CHUNK",            "TK_CHUNK",         ALWAYS,                 0},
  { "COMMITSLEEP",      "TK_COMMITSLEEP",   ALWAYS,                 1},
  { "CONSUMER",         "TK_CONSUMER",      ALWAYS,                 0},
  { "CONVERTSLEEP",     "TK_CONVERTSLEEP",  ALWAYS,                 1},
  { "COVERAGE",         "TK_COVERAGE",      ALWAYS,                 0},
  { "CRLE",             "TK_CRLE",          ALWAYS,                 0},
  { "CYCLE",            "TK_CYCLE",         ALWAYS,                 0},
  { "DATA",             "TK_DATA",          ALWAYS,                 0},
  { "DATABLOB",         "TK_DATABLOB",      ALWAYS,                 0},
  { "DDL",              "TK_DDL",           ALWAYS,                 0},
  { "DISABLE",          "TK_DISABLE",       ALWAYS,                 0},
  { "DRYRUN",           "TK_DRYRUN",        ALWAYS,                 1},
  { "ENABLE",           "TK_ENABLE",        ALWAYS,                 0},
  { "FUNCTION",         "TK_FUNCTION",      ALWAYS,                 0},
  { "GENID48",          "TK_GENID48",       ALWAYS,                 0},
  { "GET",              "TK_GET",           ALWAYS,                 0},
  { "GRANT",            "TK_GRANT",         ALWAYS,                 0},
  { "INCREMENT",        "TK_INCREMENT",     ALWAYS,                 0},
  { "IPU",              "TK_IPU",           ALWAYS,                 0},
  { "ISC",              "TK_ISC",           ALWAYS,                 0},
  { "KW",               "TK_KW",            ALWAYS,                 0},
  { "LUA",              "TK_LUA",           ALWAYS,                 0},
  { "LZ4",              "TK_LZ4",           ALWAYS,                 0},
  { "MAXVALUE",         "TK_MAXVALUE",      ALWAYS,                 0},
  { "MINVALUE",         "TK_MINVALUE",      ALWAYS,                 0},
  { "NONE",             "TK_NONE",          ALWAYS,                 0},
  { "ODH",              "TK_ODH",           ALWAYS,                 0},
  { "OFF",              "TK_OFF",           ALWAYS,                 0},
  { "OP",               "TK_OP",            ALWAYS,                 0},
  { "OPTIONS",          "TK_OPTIONS",       ALWAYS,                 0},
  { "PARTITION",        "TK_PARTITION",     ALWAYS,                 0},
  { "PASSWORD",         "TK_PASSWORD",      ALWAYS,                 0},
  { "PERIOD",           "TK_PERIOD",        ALWAYS,                 0},
  { "PROCEDURE",        "TK_PROCEDURE",     ALWAYS,                 0},
  { "PUT",              "TK_PUT",           ALWAYS,                 0},
  { "READ",             "TK_READ",          ALWAYS,                 0},
  { "REBUILD",          "TK_REBUILD",       ALWAYS,                 0},
  { "REC",              "TK_REC",           ALWAYS,                 0},
  { "RESERVED",         "TK_RESERVED",      ALWAYS,                 0},
  { "RETENTION",        "TK_RETENTION",     ALWAYS,                 0},
  { "REVOKE",           "TK_REVOKE",        ALWAYS,                 0},
  { "RLE",              "TK_RLE",           ALWAYS,                 0},
  { "ROWLOCKS",         "TK_ROWLOCKS",      ALWAYS,                 0},
  { "SCALAR",           "TK_SCALAR",        ALWAYS,                 0},
  { "SCHEMACHANGE",     "TK_SCHEMACHANGE",  ALWAYS,                 1},
  { "SELECTV",          "TK_SELECTV",       ALWAYS,                 1},
  { "SEQUENCE",         "TK_SEQUENCE",      ALWAYS,                 0},
  { "START",            "TK_START",         ALWAYS,                 0},
  { "SUMMARIZE",        "TK_SUMMARIZE",     ALWAYS,                 0},
  { "THREADS",          "TK_THREADS",       ALWAYS,                 0},
  { "THRESHOLD",        "TK_THRESHOLD",     ALWAYS,                 0},
  { "TIME",             "TK_TIME",          ALWAYS,                 0},
  { "TRUNCATE",         "TK_TRUNCATE",      ALWAYS,                 0},
  { "USERSCHEMA",       "TK_USERSCHEMA",    ALWAYS,                 0},
  { "VERSION",          "TK_VERSION",       ALWAYS,                 0},
  { "WRITE",            "TK_WRITE",         ALWAYS,                 0},
  { "ZLIB",             "TK_ZLIB",          ALWAYS,                 0},
};

/* Number of keywords */
static int nKeyword = (sizeof(aKeywordTable)/sizeof(aKeywordTable[0]));

/* Map all alphabetic characters into lower-case for hashing.  This is
** only valid for alphabetics.  In particular it does not work for '_'
** and so the hash cannot be on a keyword position that might be an '_'.
*/
#define charMap(X)   (0x20|(X))

/*
** Comparision function for two Keyword records
*/
static int keywordCompare1(const void *a, const void *b){
  const Keyword *pA = (Keyword*)a;
  const Keyword *pB = (Keyword*)b;
  int n = pA->len - pB->len;
  if( n==0 ){
    n = strcmp(pA->zName, pB->zName);
  }
  assert( n!=0 );
  return n;
}
static int keywordCompare2(const void *a, const void *b){
  const Keyword *pA = (Keyword*)a;
  const Keyword *pB = (Keyword*)b;
  int n = pB->longestSuffix - pA->longestSuffix;
  if( n==0 ){
    n = strcmp(pA->zName, pB->zName);
  }
  assert( n!=0 );
  return n;
}
static int keywordCompare3(const void *a, const void *b){
  const Keyword *pA = (Keyword*)a;
  const Keyword *pB = (Keyword*)b;
  int n = pA->offset - pB->offset;
  if( n==0 ) n = pB->id - pA->id;
  assert( n!=0 );
  return n;
}

/*
** Return a KeywordTable entry with the given id
*/
static Keyword *findById(int id){
  int i;
  for(i=0; i<nKeyword; i++){
    if( aKeywordTable[i].id==id ) break;
  }
  return &aKeywordTable[i];
}

/*
** This routine does the work.  The generated code is printed on standard
** output.
*/
int main(int argc, char **argv){
  int i, j, k, h;
  int bestSize, bestCount;
  int count;
  int nChar;
  int totalLen = 0;
  int aHash[1000];  /* 1000 is much bigger than nKeyword */
  char zText[2000];

  /* Remove entries from the list of keywords that have mask==0 */
  for(i=j=0; i<nKeyword; i++){
    if( aKeywordTable[i].mask==0 ) continue;
    if( j<i ){
      aKeywordTable[j] = aKeywordTable[i];
    }
    j++;
  }
  nKeyword = j;

  /* Fill in the lengths of strings and hashes for all entries. */
  for(i=0; i<nKeyword; i++){
    Keyword *p = &aKeywordTable[i];
    p->len = (int)strlen(p->zName);
    assert( p->len<sizeof(p->zOrigName) );
    strcpy(p->zOrigName, p->zName);
    totalLen += p->len;
    p->hash = (charMap(p->zName[0])*4) ^
              (charMap(p->zName[p->len-1])*3) ^ (p->len*1);
    p->id = i+1;
  }

  /* Sort the table from shortest to longest keyword */
  qsort(aKeywordTable, nKeyword, sizeof(aKeywordTable[0]), keywordCompare1);

  /* Look for short keywords embedded in longer keywords */
  for(i=nKeyword-2; i>=0; i--){
    Keyword *p = &aKeywordTable[i];
    for(j=nKeyword-1; j>i && p->substrId==0; j--){
      Keyword *pOther = &aKeywordTable[j];
      if( pOther->substrId ) continue;
      if( pOther->len<=p->len ) continue;
      for(k=0; k<=pOther->len-p->len; k++){
        if( memcmp(p->zName, &pOther->zName[k], p->len)==0 ){
          p->substrId = pOther->id;
          p->substrOffset = k;
          break;
        }
      }
    }
  }

  /* Compute the longestSuffix value for every word */
  for(i=0; i<nKeyword; i++){
    Keyword *p = &aKeywordTable[i];
    if( p->substrId ) continue;
    for(j=0; j<nKeyword; j++){
      Keyword *pOther;
      if( j==i ) continue;
      pOther = &aKeywordTable[j];
      if( pOther->substrId ) continue;
      for(k=p->longestSuffix+1; k<p->len && k<pOther->len; k++){
        if( memcmp(&p->zName[p->len-k], pOther->zName, k)==0 ){
          p->longestSuffix = k;
        }
      }
    }
  }

  /* Sort the table into reverse order by length */
  qsort(aKeywordTable, nKeyword, sizeof(aKeywordTable[0]), keywordCompare2);

  /* Fill in the offset for all entries */
  nChar = 0;
  for(i=0; i<nKeyword; i++){
    Keyword *p = &aKeywordTable[i];
    if( p->offset>0 || p->substrId ) continue;
    p->offset = nChar;
    nChar += p->len;
    for(k=p->len-1; k>=1; k--){
      for(j=i+1; j<nKeyword; j++){
        Keyword *pOther = &aKeywordTable[j];
        if( pOther->offset>0 || pOther->substrId ) continue;
        if( pOther->len<=k ) continue;
        if( memcmp(&p->zName[p->len-k], pOther->zName, k)==0 ){
          p = pOther;
          p->offset = nChar - k;
          nChar = p->offset + p->len;
          p->zName += k;
          p->len -= k;
          p->prefix = k;
          j = i;
          k = p->len;
        }
      }
    }
  }
  for(i=0; i<nKeyword; i++){
    Keyword *p = &aKeywordTable[i];
    if( p->substrId ){
      p->offset = findById(p->substrId)->offset + p->substrOffset;
    }
  }

  /* Sort the table by offset */
  qsort(aKeywordTable, nKeyword, sizeof(aKeywordTable[0]), keywordCompare3);

  /* Figure out how big to make the hash table in order to minimize the
  ** number of collisions */
  bestSize = nKeyword;
  bestCount = nKeyword*nKeyword;
  for(i=nKeyword/2; i<=2*nKeyword; i++){
    for(j=0; j<i; j++) aHash[j] = 0;
    for(j=0; j<nKeyword; j++){
      h = aKeywordTable[j].hash % i;
      aHash[h] *= 2;
      aHash[h]++;
    }
    for(j=count=0; j<i; j++) count += aHash[j];
    if( count<bestCount ){
      bestCount = count;
      bestSize = i;
    }
  }

  /* Compute the hash */
  for(i=0; i<bestSize; i++) aHash[i] = 0;
  for(i=0; i<nKeyword; i++){
    h = aKeywordTable[i].hash % bestSize;
    aKeywordTable[i].iNext = aHash[h];
    aHash[h] = i+1;
  }

  /* Begin generating code */
  printf("#ifndef INCLUDE_KEYWORDHASH_H \n");
  printf("#define INCLUDE_KEYWORDHASH_H \n"); 
  printf("%s", zHdr);
  printf("/* Hash score: %d */\n", bestCount);
  printf("static int keywordCode(const char *z, int n, int *pType){\n");
  printf("  /* zText[] encodes %d bytes of keywords in %d bytes */\n",
          totalLen + nKeyword, nChar+1 );
  for(i=j=k=0; i<nKeyword; i++){
    Keyword *p = &aKeywordTable[i];
    if( p->substrId ) continue;
    memcpy(&zText[k], p->zName, p->len);
    k += p->len;
    if( j+p->len>70 ){
      printf("%*s */\n", 74-j, "");
      j = 0;
    }
    if( j==0 ){
      printf("  /*   ");
      j = 8;
    }
    printf("%s", p->zName);
    j += p->len;
  }
  if( j>0 ){
    printf("%*s */\n", 74-j, "");
  }
  printf("  static const char zText[%d] = {\n", nChar);
  zText[nChar] = 0;
  for(i=j=0; i<k; i++){
    if( j==0 ){
      printf("    ");
    }
    if( zText[i]==0 ){
      printf("0");
    }else{
      printf("'%c',", zText[i]);
    }
    j += 4;
    if( j>68 ){
      printf("\n");
      j = 0;
    }
  }
  if( j>0 ) printf("\n");
  printf("  };\n");

  printf("  static const unsigned char aHash[%d] = {\n", bestSize);
  for(i=j=0; i<bestSize; i++){
    if( j==0 ) printf("    ");
    printf(" %3d,", aHash[i]);
    j++;
    if( j>12 ){
      printf("\n");
      j = 0;
    }
  }
  printf("%s  };\n", j==0 ? "" : "\n");    

  printf("  static const unsigned char aNext[%d] = {\n", nKeyword);
  for(i=j=0; i<nKeyword; i++){
    if( j==0 ) printf("    ");
    printf(" %3d,", aKeywordTable[i].iNext);
    j++;
    if( j>12 ){
      printf("\n");
      j = 0;
    }
  }
  printf("%s  };\n", j==0 ? "" : "\n");    

  printf("  static const unsigned char aLen[%d] = {\n", nKeyword);
  for(i=j=0; i<nKeyword; i++){
    if( j==0 ) printf("    ");
    printf(" %3d,", aKeywordTable[i].len+aKeywordTable[i].prefix);
    j++;
    if( j>12 ){
      printf("\n");
      j = 0;
    }
  }
  printf("%s  };\n", j==0 ? "" : "\n");    

  printf("  static const unsigned short int aOffset[%d] = {\n", nKeyword);
  for(i=j=0; i<nKeyword; i++){
    if( j==0 ) printf("    ");
    printf(" %3d,", aKeywordTable[i].offset);
    j++;
    if( j>12 ){
      printf("\n");
      j = 0;
    }
  }
  printf("%s  };\n", j==0 ? "" : "\n");

  printf("  static const unsigned char aCode[%d] = {\n", nKeyword);
  for(i=j=0; i<nKeyword; i++){
    char *zToken = aKeywordTable[i].zTokenType;
    if( j==0 ) printf("    ");
    printf("%s,%*s", zToken, (int)(14-strlen(zToken)), "");
    j++;
    if( j>=5 ){
      printf("\n");
      j = 0;
    }
  }
  printf("%s  };\n", j==0 ? "" : "\n");

  printf("  int i, j;\n");
  printf("  const char *zKW;\n");
  printf("  if( n>=2 ){\n");
  printf("    i = ((charMap(z[0])*4) ^ (charMap(z[n-1])*3) ^ n) %% %d;\n",
          bestSize);
  printf("    for(i=((int)aHash[i])-1; i>=0; i=((int)aNext[i])-1){\n");
  printf("      if( aLen[i]!=n ) continue;\n");
  printf("      j = 0;\n");
  printf("      zKW = &zText[aOffset[i]];\n");
  printf("#ifdef SQLITE_ASCII\n");
  printf("      while( j<n && ((z[j]&~0x20)==zKW[j] || z[j]==zKW[j]) ){ j++; }\n");
  printf("#endif\n");
  printf("#ifdef SQLITE_EBCDIC\n");
  printf("      while( j<n && toupper(z[j])==zKW[j] ){ j++; }\n");
  printf("#endif\n");
  printf("      if( j<n ) continue;\n");
  for(i=0; i<nKeyword; i++){
    printf("      testcase( i==%d ); /* %s */\n",
           i, aKeywordTable[i].zOrigName);
  }
  printf("      *pType = aCode[i];\n");
  printf("      break;\n");
  printf("    }\n");
  printf("  }\n");
  printf("  return n;\n");
  printf("}\n");
  printf("int sqlite3KeywordCode(const unsigned char *z, int n){\n");
  printf("  int id = TK_ID;\n");
  printf("  keywordCode((char*)z, n, &id);\n");
  printf("  return id;\n");
  printf("}\n");

  printf("#endif // INCLUDE_KEYWORDHASH_H \n");

  printf("#define SQLITE_N_KEYWORD %d\n", nKeyword);

  printf("#ifdef INCLUDE_FINALKEYWORD_H \n");
  
  printf("typedef struct {\n");
  printf("  char reserved; \n");
  printf("  char *name;\n");
  printf("} FinalKeyword_t ; \n");

  printf("FinalKeyword_t f_keywords[] = { \n");

  int namesLength = 0;
  int reservedNamesLength = 0;
  
  for(i=j=k=0; i<nKeyword; i++){
    Keyword *p = &aKeywordTable[i];


    if (p->mask)
    {
        int len = strlen(aKeywordTable[i].zOrigName) + 1;
        namesLength += len;
        if (p->reserved)
            reservedNamesLength += len;

        printf("  {%d, \"%s\"},\n", p->reserved, aKeywordTable[i].zOrigName);
    }
  }  
  printf("};\n");
  printf("#define SQLITE_KEYWORD_LEN %d\n", namesLength);
  printf("#define SQLITE_RES_KEYWORD_LEN %d\n", reservedNamesLength);
  
  printf("#endif\n");

  return 0;
}
