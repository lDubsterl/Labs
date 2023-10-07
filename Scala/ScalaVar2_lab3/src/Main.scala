import scala.util.matching.Regex

object Individual {
  def main(args: Array[String]): Unit = {
    var string = scala.io.StdIn.readLine()
    val regex = "[eyuioaEYUIOЁAУЕЫАОЭИЮёуеыаоэию]".r
    var amount = 0
    regex.findAllMatchIn(string).foreach(_ => amount += 1)
    println("Amount of vowel letters are " + amount)
  }
}

object Main {
  def main(args: Array[String]): Unit = {
    var str = "1 + 1 = 2"
    var regex = "1".r;
    str = regex.replaceAllIn(str, "one")
    regex = "2".r;
    str = regex.replaceAllIn(str, "two")
    println(str)
  }
}

object Main2 {
  def main(args: Array[String]): Unit = {
    val str = "Being quiet buetiful girl she never thought of buety"
    val regex = "bue(\\w+)".r
    regex.findAllMatchIn(str).foreach(println)
  }
}

object Main3 {
  def main(args: Array[String]): Unit = {
    var str = "one plus one is something"
    val regex = "\\bone\\b".r
    str = regex.replaceAllIn(str, "two")
    println(str)
  }
}

object Main4 {
  def main(args: Array[String]): Unit = {
    var str = "world is nice"
    val regex1 = "\\w+".r
    val regex2 = "\\w+$".r
    var word1 = regex1.findFirstIn(str).get
    var word2 = regex2.findFirstIn(str).get
    str = regex2.replaceFirstIn(str, word1)
    str = regex1.replaceFirstIn(str, word2)
    println(str)
  }
}

object Main5 {
  def main(args: Array[String]): Unit = {
    var str = "Hello to all my friends"
    val regex = "[aeiouy]".r
    str = regex.replaceAllIn(str, "")
    println(str)
  }
}

object Main6 {
  def main(args: Array[String]): Unit = {
    val str = "Hello to everybody"
    var regex = ".(.)?".r
    println(regex.replaceAllIn(str, "$1"))
  }
}

object Main7 {
  def main(args: Array[String]): Unit = {
    val str = "Hello to everybody"
    val regex = "[e]".r
    println(regex.replaceAllIn(str, ""))
  }
}

object Main8 {
  def main(args: Array[String]): Unit = {
    val str = "Be healthy"
    var regex = " ".r
    println(regex.replaceFirstIn(str, " always "))
  }
}

object Spaces{
  def main(args: Array[String]): Unit = {
    var string = scala.io.StdIn.readLine()
    val regex = " ".r
    println(regex.replaceAllIn(string, ""))
  }
}

object letters{
  def main(args: Array[String]): Unit = {
    var string = scala.io.StdIn.readLine()
    val regex = "t".r
    println(regex.replaceAllIn(string, "tttt"))
  }
}
//набрать предложение, удалить пробелы. дано слово father, дублировать t до 4