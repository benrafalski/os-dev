

// A utility function to reverse a string
void reverse(char str[], int length)
{
    int start = 0;
    int end = length - 1;
    while (start < end)
    {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        end--;
        start++;
    }
}
char* citoa(uint64_t value, char* buffer, int base) {
    const char* digits = "0123456789abcdef";
    char* ptr = buffer;
    char* head = buffer;

    if (value == 0) {
        *ptr++ = '0';
        *ptr = '\0';
        return buffer;
    }

    while (value > 0) {
        *ptr++ = digits[value % base];
        value /= base;
    }
    *ptr = '\0';

    // Reverse the string in-place
    for (--ptr; head < ptr; head++, ptr--) {
        char tmp = *head;
        *head = *ptr;
        *ptr = tmp;
    }

    return buffer;
}
// char *citoa(long long int num, char *str, int base)
// {
//     int i = 0;
//     int isNegative = 0;

//     /* Handle 0 explicitly, otherwise empty string is
//      * printed for 0 */
//     if (num == 0)
//     {
//         str[i++] = '0';
//         str[i] = '\0';
//         return str;
//     }

//     // In standard itoa(), negative numbers are handled
//     // only with base 10. Otherwise numbers are
//     // considered unsigned.
//     if (num < 0 && base == 10)
//     {
//         isNegative = 1;
//         num = -num;
//     }

//     // Process individual digits
//     while (num != 0)
//     {
//         int rem = num % base;
//         str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
//         num = num / base;
//     }

//     // If number is negative, append '-'
//     if (isNegative)
//         str[i++] = '-';

//     str[i] = '\0'; // Append string terminator

//     // Reverse the string
//     reverse(str, i);

//     return str;
// }

int strlen(const char* str) {
    int len = 0;
    while (*str != '\0') {
        len++;
        str++;
    }
    return len;
}

char *strcat(char* dest, const char* src){
    char * tmp = dest;
    while (*tmp != '\0') {
        tmp++;  
    }
    while(*src != '\0'){
        *tmp = *src;
        tmp++;
        src++;
    }

    *tmp='\0';
    return dest;
}

// 0 if not equal, 1 if equal
int strcmp(const char* s1, const char* s2){
    while(*s1 != '\0' && *s2 != '\0'){
        if(*s1 != *s2){
            return (*s1 - *s2);  // Return difference for unequal
        }
        s1++;
        s2++;
    }
    
    // Handle case where strings are different lengths
    return (*s1 - *s2);
}

char* memcpy(char* src, char* dest, uint32_t length){
    char* _src = src;
    char* _dest = dest;

    for(uint32_t i = 0; i < length;i++){
        *_dest = *_src;
        _src++;
        _dest++;
    }

    return dest;
}

void memset(char* src, char new, uint32_t length){
    char* _src = src;
    for(uint32_t i = 0; i < length;i++){
        *_src = new;
        _src++;
    }
}

char* strncpy (char *s1, const char *s2, int n) {
  int size = strlen(s2);
  if(size != n){
    memset(s1 + size, '\0', n - size);
  }
    
  return memcpy(s1, (char*)s2, size);
}

char* strcpy(char* dest, const char* src) {
    char* original_dest = dest;
    while (*src != '\0') {
        *dest = *src;
        dest++;
        src++;
    }
    *dest = '\0'; // Null terminate the destination string
    return original_dest;
}

uint32_t strsplit(char* str, const char c){
    uint32_t i = 0, n = 0;
    while(str[i]){
        if(str[i] == c){
            n++;
            str[i] = '\0';
        }
        i++;
    }
    return n;
}


uint32_t strends_with(char* str, const char c){
    char* _name = str;
    while(*_name){
        _name++;
    }
    _name--;
    if(*_name == c){
        return 1;
    }
    return 0;
}


uint32_t strstarts_with(char* str, const char c){
    if(*str == c){
        return 1;
    }
    return 0;
}

// Helper function to find character in string
char* strchr(const char* str, int c) {
    while (*str) {
        if (*str == c) {
            return (char*)str;
        }
        str++;
    }
    return (*str == c) ? (char*)str : NULL;
}

char* strtok(char* str, const char* delim) {
    static char* last = NULL;
    
    if (str) {
        last = str;
    } else if (!last) {
        return NULL;
    }
    
    // Skip leading delimiters
    while (*last && strchr(delim, *last)) {
        last++;
    }
    
    if (!*last) {
        return NULL;
    }
    
    char* start = last;
    
    // Find next delimiter
    while (*last && !strchr(delim, *last)) {
        last++;
    }
    
    if (*last) {
        *last++ = '\0';
    }
    
    return start;
}



