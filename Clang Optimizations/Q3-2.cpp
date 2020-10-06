#include <vector>
using namespace std;

vector<int> callV() 
{
  return vector<int>(2, 2);
}

int main() {
  for (int i = 0; i<2000000000; ++i)
  {
    callV();
  }
}