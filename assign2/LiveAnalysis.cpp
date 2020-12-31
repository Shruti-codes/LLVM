#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Function.h"
#include <llvm/ADT/DenseMap.h>
#include "llvm/ADT/DenseSet.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/Instruction.h"
#include "llvm/ADT/SmallPtrSet.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/IR/Argument.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/IntrinsicInst.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/IR/Use.h"
#include "llvm/Transforms/Utils/Local.h"
#include <iterator>
#include <utility>
#include <vector>

using namespace llvm;

namespace
{
  class Liveness {
    private:
      //map pointers to pointers: DenseMap(key, value)
       DenseMap<const Instruction*, DenseSet<Value*>> In, Out, OutKill;
       DenseSet<Value*> OldIn;

    public:
        bool run(Function &F)
        {
          //items in the worklist are basic blocks
          SmallVector<BasicBlock*, 32> wL;
          bool change = true;

          while(change)
          {
            change = false;

            //finding all basic blocks in function
            for(BasicBlock* BB : depth_first(&F.getEntryBlock()))
              wL.push_back(BB);

            while(!wL.empty())
            {
              BasicBlock* BB = wL.back();
              wL.pop_back();

              // Take the union of all successors
              for(BasicBlock* Succ: successors(BB))
              {
                  for(Value* V: In[&(*(Succ->begin()))])
                      Out[BB->getTerminator()].insert(V);
              }

              for(Value* V: OutKill[BB->getTerminator()])
                  Out[BB->getTerminator()].erase(V);

              auto FI = BB->begin();
              Instruction* FInsn = &(*FI);
              for(Value* V: In[FInsn])
                OldIn.insert(V);

              //determining what values are used by an instruction
              for(auto OI = BB->rbegin(); OI != BB->rend(); OI++)
              {
                Instruction* Insn = &(*OI);
                DenseSet<Value*> gen, kill;
                if(OI != BB->rbegin())
                {
                    auto temp_I = OI;
                    temp_I--;
                    Out[Insn] = In[&*temp_I];
                }

              
                Value* val;
                if(isa<StoreInst>(*Insn))
                {
                  val = Insn->getOperand(1);
                  kill.insert(val);

                  Value* op = Insn->getOperand(0);
                  if(ConstantInt *CI = dyn_cast<ConstantInt>(op));
                  else gen.insert(op);
                }
                else
                {
                  val = dyn_cast<Value>(Insn);
                  kill.insert(val);

                  for(auto &op: Insn->operands())
                  {
                    if(isa<Instruction>(&op) || isa<Argument>(&op))
                      gen.insert(op);
                  }

                  if(PHINode* phiI = dyn_cast<PHINode>(Insn)) 
                  {
                    for(BasicBlock* Pred: predecessors(BB))
                    {
                      for(auto &op: Insn->operands()) 
                      {
                        if(isa<Instruction>(&op) || isa<Argument>(&op))
                          OutKill[Pred->getTerminator()].insert(op);
                      }
                    }

                    //we ignore the program points before phi instructions as the liveness sets are not well defined at these points.
                    for(int i = 0; i < phiI->getNumIncomingValues(); i++)
                    {
                      Value* op = phiI->getIncomingValue(i);
                      if(isa<Instruction>(op) || isa<Argument>(op))
                      {
                        BasicBlock* Pred = phiI->getIncomingBlock(i);
                        OutKill[Pred->getTerminator()].erase(op);
                      }
                    }
                  }
                }

              In[Insn] = Out[Insn];
              if(!kill.empty())
              {
                for(auto V: kill)
                {
                    In[Insn].erase(V);
                }
              }

              if(!gen.empty())
              { 
                  for(auto V: gen)
                  {
                    In[Insn].insert(V);
                  }
              }
              kill.clear();
              gen.clear();
            }

            for(Value* V: In[FInsn])
            {
                if(OldIn.count(V) == 0)
                {
                    change = true;
                    break;
                }
            }
            OldIn.clear();
          }
        }

      //  Iterate over the basic blocks in the function
        outs() << "Live values out of each Basic Block\n";
        outs() << "Basic Block \t: Live Values\n";
        for(Function::iterator b = F.begin(); b != F.end(); ++b)
        {
          BasicBlock* BB = &*b;
          outs() << BB->getName() << " \t: ";
          for(auto V: Out[BB->getTerminator()])
          {
            outs() << V->getName() << ",";
          }
          outs() << '\n';
        }
        outs() << '\n';

        std::map<int, int> hist;
        int i1 = 0, i2 = 0;
        outs() << "Live values at each program point in each Basic Block\n";
        outs() << "Program Point \t: Live Values\n";
        for(Function::iterator b = F.begin(); b != F.end(); b++)
        {
          BasicBlock* BB = &*b;
          i2 = 0;
          for(BasicBlock::iterator i = BB->begin(); i != BB->end(); ++i)
          {
              Instruction* I = &*i;
              hist[Out[I].size()]++;
              outs() <<BB->getName()<< i2<< " \t: ";
              for(auto V: Out[I])
              {
                  outs() << V->getName() << ",";
              }
              outs() << '\n';
              i1++;
              i2++;
          }
        }
        outs() << '\n';

        outs() << "Histogram\n";
        outs() << "#Live Values \t: #Program Points\n";
        for(auto it: hist)
        {
          outs() << it.first << " \t\t: " << it.second << '\n';
        }
          return false;
        }
  };

   class LVAPass : public FunctionPass
  {
    public:
      static char ID;
      LVAPass() : FunctionPass(ID) {}

      //overrides an abstract virtual method inherited from FunctionPass.
      virtual bool runOnFunction(Function &F) override
      {
          Liveness L;
          L.run(F);
          return false;
      }
  };
}

char LVAPass::ID = 0;

  //  - give it a command-line argument
  //  - a name
  //  - a flag saying that we don't modify the CFG
  //  - a flag saying this is not an analysis pass
static RegisterPass<LVAPass> X("lva", "Live Variable Analysis in LLVM", false, true);
