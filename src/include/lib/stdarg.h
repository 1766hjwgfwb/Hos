#ifndef _STDARG_H
#define _STDARG_H



typedef char *va_list;

#define _ALIGN_SIZE(type) (((sizeof(type) + sizeof(int) - 1) / sizeof(int)) * sizeof(int))

#define va_start(ap, last_arg) (ap = (va_list)&last_arg + _ALIGN_SIZE(last_arg))
#define va_arg(ap, type) (*(type *)((ap += _ALIGN_SIZE(type)) - _ALIGN_SIZE(type)))
#define va_end(ap) (ap = (va_list)0)










#endif /* _STDARG_H */