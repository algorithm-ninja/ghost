#include "file/file.h"
#include "util.h"
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

File::File(const std::string &path, bool readonly) {
  SYSCALL(fd_ = open(path.c_str(), readonly ? O_RDONLY : O_RDWR | O_CREAT,
                     S_IRWXU));
  SYSCALL(len_ = lseek(fd_, 0, SEEK_END));
  SYSCALL(lseek(fd_, 0, SEEK_SET));
  SYSCALL_F(addr_ = (char *)mmap(nullptr, len_,
                                 readonly ? PROT_READ : PROT_READ | PROT_WRITE,
                                 MAP_SHARED, fd_, 0),
            nullptr);
}

File::~File() {
  if (fd_ == -1)
    return;
  SYSCALL(munmap(addr_, len_));
  close(fd_);
}
