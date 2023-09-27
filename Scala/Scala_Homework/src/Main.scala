import scala.collection.{SortedMap, mutable}
import scala.util.Random
import scala.util.control.Breaks.breakable

object Main {

  def generator(size: Int): Array[Float] = {
    var map: SortedMap[Int, List[Float]] = SortedMap.empty
    var rand = Random()
    //var seed = rand.nextLong()
    //rand.setSeed(seed)
    var sectionAmount: Int = size / rand.nextInt(size - 1) + 1
    //println(seed)
    var sectionSize: Float = 1f / sectionAmount
    var amountOfNumbersInSection: Int = size / sectionAmount
    //print("Sections amount: " + sectionAmount + '\n' + "Section size: " + sectionSize + '\n')
    if (amountOfNumbersInSection * sectionAmount < size)
      {
        sectionAmount += 1
        sectionSize = 1f / sectionAmount
      }
    var sections: Array[Int] = Array.fill(sectionAmount) {
      0
    }
    for (i <- 0 until sectionAmount)
      map += i -> List()
    print("Sections amount: " + sectionAmount + '\n' + "Section size: " + sectionSize + '\n')
    print("Amount of numbers in section: " + amountOfNumbersInSection + '\n')
    var section: Int = 0
    for (i <- 0 until size) {
      breakable {
        while (true) {
          var number = Random.nextFloat()
          section = (number / sectionSize).toInt
          var list: List[Float] = map.apply(section)
          if (sections.apply(section) < amountOfNumbersInSection) {
            list = list.appended(number)
            map += section -> list
            sections.update(section, sections.apply(section) + 1)
            scala.util.control.Breaks.break()
          }
        }
      }
    }
    println(sections.toList)
    for ((k, v) <- map) {
      println("Section " + k * sectionSize + " - " + (k * sectionSize + sectionSize) + ": " + v)
    }
    var arr: Array[Float] = Array.empty
    for ((k, v) <- map) {
      for (i <- v.indices)
        arr = arr.appended(v.apply(i))
    }
    arr
  }

  def main(args: Array[String]): Unit = {
    var size = 20
    var arr: Array[Float] = generator(size)
    println("Array: ")
    for (i <- arr.indices) {
      print(arr.apply(i) + " ")
      if (i % (size / 5) == 0 && i != 0)
        println("")
    }
  }
}