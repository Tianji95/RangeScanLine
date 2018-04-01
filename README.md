# RangeScanLine
using only CPU(without directX or onengl) to draw an object which is more than 1000 faces, basing on  rangeScanLine algorithm, this algorithm is much more faster than the software-rendering that I had write.
on ryzen7 1700, we can get more than 110 frames per second to render more that 1000 faces, while the software-rendering can only get 100 frames per second to render only 12 faces;

**this is what I rendered**

![scene](https://github.com/ZJUZTJ/RangeScanLine/blob/master/example.png)
