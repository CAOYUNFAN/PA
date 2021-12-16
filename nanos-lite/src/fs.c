#include <fs.h>

typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  size_t open_offset;
  ReadFn read;
  WriteFn write;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB, FD_EVENTS, FD_DISPINFO};

size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

extern size_t serial_write(const void *buf, size_t offset, size_t len);
extern size_t events_read(void *buf, size_t offset, size_t len);
extern size_t dispinfo_read(void *buf, size_t offset, size_t len);
extern size_t fb_write(const void *buf, size_t offset, size_t len);
/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  [FD_STDIN]  = {"stdin", 0, 0, 0, invalid_read, invalid_write},
  [FD_STDOUT] = {"stdout", 0, 0, 0, invalid_read, serial_write},
  [FD_STDERR] = {"stderr", 0, 0, 0, invalid_read, serial_write},
  [FD_FB] = {"/dev/fb", 0, 0, 0, invalid_read, fb_write},
  [FD_EVENTS] ={"/dev/events", 0, 0, 0, events_read, invalid_write},
  [FD_DISPINFO]= {"/dev/dispinfo",0,0,0,dispinfo_read,invalid_write},
#include "files.h"
};

static int total_size=sizeof(file_table)/sizeof(Finfo);
int fs_open(const char *pathname, int flags, int mode);
size_t fs_read(int fd, void *buf, size_t len);
size_t fs_write(int fd, const void *buf, size_t len);
size_t fs_lseek(int fd, size_t offset, int whence);
int fs_close(int fd);

static inline void check_files(int fd){
  if(fd>=total_size) panic("No such file!");
  return;
}

static inline size_t Min(size_t a,size_t b) {return a<b?a:b;}

static inline void check_filerange(int fd,size_t * len){
  if(!(file_table[fd].open_offset>=0&&file_table[fd].open_offset<=file_table[fd].size)){
    Log("open_offset=%d,size=%d",file_table[fd].open_offset,file_table[fd].size);
    assert(false);
  };
  *len=Min(*len,file_table[fd].size-file_table[fd].open_offset);
  return;
}

void init_fs() {
  // TODO: initialize the size of /dev/fb
  struct AM_GPU_CONFIG_T{
    bool present, has_accel; int width, height, vmemsz;
  } cfg;
  extern void __am_gpu_config(struct AM_GPU_CONFIG_T *cfg);
  __am_gpu_config(&cfg);
//  Log("width=%d,height=%d",cfg.width,cfg.height);
  file_table[FD_FB].size=cfg.width*cfg.height*sizeof(uint32_t);
  return;
}

extern size_t ramdisk_read(void *buf, size_t offset, size_t len);
extern size_t ramdisk_write(const void *buf, size_t offset, size_t len);

int fs_open(const char *pathname, int flags, int mode){
  for(int i=0;i<total_size;++i)
  if(strcmp(file_table[i].name,pathname)==0){
    file_table[i].open_offset=0;
    if(strncmp(pathname,"/share/",strlen("/share/"))==0) Log("File %s,id %d,size %u, offset %u",file_table[i].name,i,file_table[i].size,file_table[i].disk_offset);
    return i;
  }
  Log("File %s not found!",pathname);
  return -1;
}

int fs_close(int fd){
  check_files(fd);
//  Log("Close file %s",file_table[fd].name);
  return 0;
}

size_t fs_read(int fd, void *buf, size_t len){
  check_files(fd);
//  Log("Read from file %s,with len %u,now %u",file_table[fd].name,len,file_table[fd].disk_offset+file_table[fd].open_offset);
  if(file_table[fd].size) check_filerange(fd,&len);
  if(file_table[fd].read!=NULL){
    size_t temp=file_table[fd].read(buf,file_table[fd].open_offset,len);
    file_table[fd].open_offset+=temp;
    return temp;
  }else{
    ramdisk_read(buf,file_table[fd].disk_offset+file_table[fd].open_offset,len);
//    if(len==1024) for(int i=0;i<len;++i) printf("%c",*((char *)buf+i));
    file_table[fd].open_offset+=len;
    return len;
  }
}

size_t fs_write(int fd, const void *buf, size_t len){
  check_files(fd);
//  Log("Print to file %s,len %d",file_table[fd].name,len);
  if(file_table[fd].size) check_filerange(fd,&len);
//  static char temp[100000];
//  memcpy(temp,buf,len);
  if(file_table[fd].write!=NULL){
    size_t temp=file_table[fd].write(buf,file_table[fd].open_offset,len);
    file_table[fd].open_offset+=temp;
    return temp;
  }else{
    ramdisk_write(buf,file_table[fd].disk_offset+file_table[fd].open_offset,len);
    file_table[fd].open_offset+=len;
    return len;
  }
}

size_t fs_lseek(int fd, size_t offset, int whence){
  check_files(fd);
  switch (whence){
    case SEEK_SET: file_table[fd].open_offset=offset; break;
    case SEEK_CUR: file_table[fd].open_offset+=offset; break;
    case SEEK_END: file_table[fd].open_offset=file_table[fd].size+offset; break;
    default: panic("Unexpected whence=%d",whence); break;
  }
  return file_table[fd].open_offset;
}