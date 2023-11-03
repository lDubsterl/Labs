object Main1 {
  def main(args: Array[String]): Unit = {
    val text = "A mutable sequence of characters.\nThis class provides an API compatible with StringBuffer, but with no guarantee of synchronization.\nThis class is designed for use as a drop-in replacement for StringBuffer in places where the string buffer was being used by a single thread (as is generally the case).\nWhere possible, it is recommended that this class be used in preference to StringBuffer as it will be faster under most implementations."
    val stopWords = Set("a", "and", "for", "or", "of", "but", "the")
    val lines = text.split("\n")
    var filtered = lines.flatMap(line => line.split("\\W+"))
    filtered = filtered.filter(word => !stopWords.contains(word))
    filtered.foreach(println)
  }
}

object Main2 {
  def main(args: Array[String]): Unit = {
    val text = "A mutable sequence of characters.\nThis class provides an API compatible with StringBuffer, but with no guarantee of synchronization.\nThis class is designed for use as a drop-in replacement for StringBuffer in places where the string buffer was being used by a single thread (as is generally the case).\nWhere possible, it is recommended that this class be used in preference to StringBuffer as it will be faster under most implementations. programming."
    val lines = text.split("\n")
    var filtered = lines.flatMap(line => line.split("\\W+"))
    filtered.foreach(word => if (word.contains("prog")) println(word))
  }
}

object Main3
{
  def main(args: Array[String]): Unit = {
    val text = "A mutable sequence of characters.\nThis class provides an API compatible with StringBuffer, but with no guarantee of synchronization.\nThis class is designed for use as a drop-in replacement for StringBuffer in places where the string buffer was being used by a single thread (as is generally the case).\nWhere possible, it is recommended that this class be used in preference to StringBuffer as it will be faster under most implementations. programming."
    val lines = text.split("\n")
    var filtered = lines.flatMap(line => line.split("\\W+"))
    filtered.foreach(word => if (word.matches("\\w+ion")) println(word))
  }
}

object Main4 {
  def main(args: Array[String]): Unit = {
    val text = "A mutable sequence of characters.\nThis class provides an API compatible with StringBuffer, but with no guarantee of synchronization.\nThis class is designed for use as a drop-in replacement for StringBuffer in places where the string buffer was being used by a single thread (as is generally the case).\nWhere possible, it is recommended that this class be used in preference to StringBuffer as it will be faster under most implementations. programming."
    val lines = text.split("\n")
    var filtered = lines.flatMap(line => line.split("\\W+"))
    filtered.foreach(word => if (word.matches("\\w{2}r\\w*?")) println(word))
  }
}

object Main5 {
  def main(args: Array[String]): Unit = {
    val text = "A mutable sequence of characters.\nThis class provides an API compatible with StringBuffer, but with no guarantee of synchronization.\nThis class is designed for use as a drop-in replacement for StringBuffer in places where the string buffer was being used by a single thread (as is generally the case).\nWhere possible, it is recommended that this class be used in preference to StringBuffer as it will be faster under most implementations. programming."
    val lines = text.split("\n")
    var filtered = lines.flatMap(line => line.split("\\W+"))
    filtered.foreach(word => if (word.matches("\\w*?ee\\w*?")) println(word))
  }
}

object Main7 {
  def main(args: Array[String]): Unit = {
    val text = "A mutable sequence of characters.\nThis class provides an API compatible with StringBuffer, but with no guarantee of synchronization.\nThis class is designed for use as a drop-in replacement for StringBuffer in places where the string buffer was being used by a single thread (as is generally the case).\nWhere possible, it is recommended that this class be used in preference to StringBuffer as it will be faster under most implementations. programming."
    val lines = text.split("\n")
    var filtered = lines.flatMap(line => line.split("\\W+"))
    println(filtered.apply(filtered.length - 1))
  }
}


object Def1 {
  def main(args: Array[String]): Unit = {
    var text = "A mutable sequence of characters.\nThis class provides an API compatible with StringBuffer, but with no guarantee of synchronization.\nThis class is designed for use as a drop-in replacement for StringBuffer in places where the string buffer was being used by a single thread (as is generally the case).\nWhere possible, it is recommended that this class be used in preference to StringBuffer as it will be faster under most implementations. programming."
    //val lines = text.split("\n")
    //var filtered = lines.flatMap(line => line.split("\\W+"))
    var regex = "\\w{2}\\b".r
    println(regex.replaceAllIn(text, "ing"))
  }
}

object Def2 {
  def main(args: Array[String]): Unit = {
    var text = "A mutable sequence of characters.\nThis class provides an API compatible with StringBuffer, but with no guarantee of synchronization.\nThis class is designed for use as a drop-in replacement for StringBuffer in places where the string buffer was being used by a single thread (as is generally the case).\nWhere possible, it is recommended that this class be used in preference to StringBuffer as it will be faster under most implementations. programming."
    var regex = "[aeiouyAEIOUY]".r
    println(regex.replaceAllIn(text, ""))
  }
}

object Def3 {
  def main(args: Array[String]): Unit = {
    var text = "A mutable sequence of characters." +
      "\nThis class provides an API compatible with StringBuffer, but with no guarantee of synchronization." +
      "\nThis class is designed for use as a drop-in replacement for StringBuffer in places where the string buffer" +
      " was being used by a single thread (as is generally the case).\nWhere possible, it is recommended that this " +
      "class be used in preference to StringBuffer as it will be faster under most implementations. programming. the"
    val stopWords = "\\ba\\b|\\band\\b|\\bthe\\b|\\bof\\b".r
    println(stopWords.replaceAllIn(text, ""))
    }
}
// есть 2 предложения. сделать во всех словах ing окончания; удалить стоп слова; удалить все гласные;