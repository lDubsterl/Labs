object Main {
  def main(args: Array[String]): Unit = {
    var str = "1+1=2"
    var regex = "1".r;
    str = regex.replaceAllIn(str, " one ")
    regex = "2".r
    str = regex.replaceAllIn(str, " two")
    println(str)
  }
}