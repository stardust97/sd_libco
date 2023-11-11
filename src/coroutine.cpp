#include "coroutine.h"

#include <cstring>
#include <cassert>

namespace mylibco{

Coroutine::Coroutine(): coroutine_id_{g_coroutine_id_++}, stack_{nullptr},
    stack_size_{0} {
  // 主协程不需要创建栈，直接使用线程的栈
  main_coroutine_ = this;
  cur_coroutine_ = this;
  memset(&contex_, 0, sizeof(coctx));
  // 不需要为主协程的m_coctx设置初值，
  // 因为后面协程切换的时候会保存当前寄存器上下文到主协程的contex_中的。
}

Coroutine::Coroutine(int stack_size, Task callback) : coroutine_id_{g_coroutine_id_++},
    stack_{nullptr}, stack_size_{stack_size}, callback_(callback){
  if(!main_coroutine_) {
    main_coroutine_ = new Coroutine();
  }
  assert(main_coroutine_ != nullptr);
  stack_ = static_cast<char*>(malloc(stack_size));
  char* top = stack_ + stack_size_; // 注意栈是向下增长的,堆相反
  // 由于64位系统对栈的操作(push、pop)都是8字节对齐的
  // 所以需要对栈顶进行8字节对齐
  top = reinterpret_cast<char*> ( 
      (reinterpret_cast<unsigned long>(top) & -16LL) );
  memset(&contex_, 0, sizeof(coctx));
  contex_.regs[KRSP] = top;
  contex_.regs[KRBP] = top;
  // 从协程首先执行co_functino函数
  contex_.regs[KRETAddr] = &co_fuction; // TODO 能否使用std::bind，就不用使用statci了
  contex_.regs[KRDI] = this;
  

}


void Coroutine::Resume(Coroutine* co) {

}

void Coroutine::Yeild() {

}

void Coroutine::co_fuction(Coroutine* co) {
  if(co -> callback_) { // 私有变量？
    co -> callback_(); // 执行从协程的任务
  }
  Yeild(); // 执行完从协程的任务后，将CPU还给主协程
}


} // namespace mylibco


