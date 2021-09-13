#include <isa.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>

enum {
  TK_NOTYPE = 256, TK_EQ,

  /* TODO: Add more token types */
  TK_NUM,TK_NEGATIVE,TK_POSITIVE,TK_HEXNUM,TK_REG
};

static struct rule {
  const char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  {"\\+", '+'},         // plus
  {"==", TK_EQ},        // equal
  
  {"\\-",'-'},			//minus
  {"\\*",'*'},
  {"\\/",'/'},
  {"[0-9]+",TK_NUM},
  {"\\(",'('},
  {"\\)",')'}
};

#define NR_REGEX ARRLEN(rules)

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

static Token tokens[100000] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);

        position += substr_len;

         /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) {
		  case TK_NOTYPE:break;
          default:{
			tokens[nr_token].type=rules[i].token_type;
			strncpy(tokens[nr_token++].str,substr_start,substr_len);
			break;
		  }
        }

        break;
      }
    }

     if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

void pre_check_parenteses(bool *success){
	int temp=0;
	for(int i=0;i<nr_token;++i){
		if(tokens[i].type=='(') ++temp;
		if(tokens[i].type==')') --temp;
		if(temp<0){*success=0;return;}

		if(tokens[i].type=='+'||tokens[i].type=='-')
			if(i==0||(tokens[i-1].type!=TK_NUM && tokens[i-1].type!=TK_HEXNUM && tokens[i-1].type!=TK_REG && tokens[i-1].type!=')')) tokens[i].type=(tokens[i].type=='+')?TK_POSITIVE:TK_NEGATIVE;
		printf("%d\n",i);
	}
	return;
}

word_t eval(int p,int q,bool *success);

word_t expr(char *e, bool *success) {
  *success=1;
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
  pre_check_parenteses(success);
  word_t ans;
  if(!*success){
	printf("Unmatched parentheses!\n");
	return 0;
  }
  ans=eval(0,nr_token-1,success);
  if(*success) return ans;
  else{
	printf("There exists mathematical errors. Fail to cacluate.\n");
    return 0;	
  }
}

bool check_parenteses(int p,int q){
	if(tokens[p].type!='('||tokens[q].type!=')') return 0;
	int temp=1,i=p+1;
	while(temp&&i<=q){
		if(tokens[i].type=='(') ++temp;
		if(tokens[i].type==')') --temp;
	}
	if(i==q) return 1;
	return 0;
}

static int get(int x){
	if(x==TK_POSITIVE||x==TK_NEGATIVE) return 1;
	if(x=='*'||x=='/') return 2;
	if(x=='+'||x=='-') return 3;

	return -1;
}

word_t eval(int p,int q,bool *success){
	if(p>q) assert(0);
 	if(p==q){
		word_t ans;
		sscanf(tokens[p].str,"%d",&ans);
		return ans;
	} 
	if(check_parenteses(p,q)){
		return eval(p+1,q-1,success);
	}
	int pos=q+1,prio=0;
	for(int i=q;i>=p;--i){
		if(tokens[i].type==TK_NUM||tokens[i].type==TK_HEXNUM||tokens[i].type==TK_REG) continue;
		int temp=get(tokens[i].type);
		if(temp>prio) prio=temp,pos=i;
	}
	if(prio==1){
		while(pos>p&&get(tokens[pos-1].type)==2) --pos;
		word_t val=eval(pos+1,q,success);
		switch(tokens[pos].type){
			case TK_POSITIVE:return val;
			case TK_NEGATIVE:return -val;
			default:assert(0);
		}
	}
	word_t val1=eval(p,pos-1,success);
	word_t val2=eval(pos+1,q,success);
	switch(tokens[pos].type){
		case '+': return val1+val2;
		case '-': return val1-val2;
		case '*': return val1*val2;
		case '/': if(val2==0) {*success=false;return 0;}else return val1/val2;
		default:assert(0);
	}
}
