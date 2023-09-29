import scala.math.abs
import scala.sys.exit

object Main {
  def main(args: Array[String]): Unit = {
    val list = List(-1, 0, 1, -2, 2, 3, 4, 6, 10 ,5)
    val sum = list.filter(num => abs(num) <= 5).sum
    println(sum)
  }
}

object Main2 {
  private def sum(list: List[Int], i: Int): Int = {
    var accum = 0;
    if (i == list.size)
      return accum
    if (i % 2 != 0)
      accum = list.apply(i)
    accum += sum(list, i + 1)
    accum
  }

  def main(args: Array[String]) = {
    val list = List(-1, 0, 1, -2, 2, 3, 4, 6, 10, 5)
    val accum = sum(list, 0)
    println(accum)
  }
}

object Main3 {
  private def middle(list: List[Int], criteria: Float): Int = {
    var dif: Float = abs(list.head - criteria)
    var elem = 0
    for (i <- list.indices if abs(list.apply(i) - criteria) < dif)
      {
        elem = list.apply(i)
        dif = abs(list.apply(i) - criteria)
      }
    elem
  }
  def main(args: Array[String]) = {
    val list = List(-1, 0, 1, -2, 2, 3, 4, 6, 10, 5)
    val sum = list.sum.toFloat / list.size
    var elem = middle(list, sum)
    println(elem)
  }
}

@scala.annotation.nowarn
object Main4 {
  def main(args: Array[String]): Unit = {
    val list = List(-1, 0, 1, -2, 2, 3, 4, 6, 10, 5)
    for (i <- 0 until list.size)
      for (j <- i + 1 until list.size)
        if (scala.math.pow(i.toDouble, 2) == list.apply(j)) {
          println("List contains square of one of its elements")
          return
        }
    println("List not contains square of one of its elements")
  }
}

object Main5 {
  private def collisions(list: List[Int], element: Int, counter: Int): Int = {
    var counter2 = 0
    if (list.isEmpty)
      return counter
    if (list.contains(element))
      counter2 = collisions(list.tail, list.head, counter + 1)
    else
      counter2 = collisions(list.tail, list.head, counter)
    counter2
  }

  def main(args: Array[String]) = {
    var counter = 0
    val list = List(-1, 0, 1, -2, 2, 3, 4, 6, 5, 5)
    counter = collisions(list.tail, list.head, 0)
    if (counter > 6)
      println("No")
    else
      println("Yes")
  }
}