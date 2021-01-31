

String getIcons(String cond_code) {  //获取天气图标
  if (cond_code == "100" || cond_code == "0" || cond_code == "2") { //白天晴
    return "B";
  }
  if (cond_code == "150" || cond_code == "1" ) { //夜晚 Clear
    return "C";
  }
  if (cond_code == "999") {//N/A
    return ")";
  }
  if (cond_code == "9" || cond_code == "4" ) { //多云或者用D
    return "A";
  }
  if (cond_code == "500" || cond_code == "30" ) { //薄雾
    return "E";
  }
  if (cond_code == "503" || cond_code == "504" || cond_code == "507" || cond_code == "508" || cond_code == "26" || cond_code == "27" || cond_code == "28" || cond_code == "29") {//扬沙//浮尘//沙尘暴//强沙尘暴
    return "F";
  }
  if (cond_code == "499" || cond_code == "901") {//雪
    return "G";
  }
  if (cond_code == "102" || cond_code == "103" || cond_code == "5" ) { //晴间多云
    return "H";
  }
  if (cond_code == "153" || cond_code == "8" ) { //夜晚气象，阴
    return "I";
  }
  //     if (cond_code == "153") {//夜晚气象，阴
  //    return "4";
  //  }
  //  if (cond_code == "154") {//夜晚气象，阴
  //    return "5";
  //  }
  if (cond_code == "502" || cond_code == "511" || cond_code == "512" || cond_code == "513" || cond_code == "31") {//霾//中度霾//重度霾//严重霾
    return "L";
  }
  if (cond_code == "501" || cond_code == "509" || cond_code == "510" || cond_code == "514" || cond_code == "515" || cond_code == "30" || cond_code == "315" || cond_code == "316" || cond_code == "317" || cond_code == "318" || cond_code == "18") { //雾//浓雾//强浓雾//大雾//特强浓雾
    return "M";
  }
  if ( cond_code == "104" || cond_code == "154" || cond_code == "7"  ) { //白天阴
    return "N";
  }
  if (cond_code == "302" || cond_code == "12" ) { //雷阵雨
    return "O";
  }
  if (cond_code == "303") {//雷阵雨//强雷阵雨
    return "P";
  }
  if (cond_code == "305" || cond_code == "308" || cond_code == "309" || cond_code == "314" || cond_code == "399" || cond_code == "13" ) { //小雨//极端降雨//毛毛雨/细雨//小到中雨//雨
    return "Q";
  }
  if (cond_code == "306" || cond_code == "307" || cond_code == "310" || cond_code == "311" || cond_code == "312" || cond_code == "14" ) {//中雨//大雨//暴雨//大暴雨//特大暴雨//中到大雨
    return "R";
  }
  //  if (cond_code == "315" || cond_code == "316" || cond_code == "317" || cond_code == "318" || cond_code == "18" ) { //大到暴雨//暴雨到大暴雨//大暴雨到特大暴雨
  //    return "8";//显示黑色图标不可见，
  //  }
  if (cond_code == "200" || cond_code == "201" || cond_code == "202" || cond_code == "203" || cond_code == "204" || cond_code == "205" || cond_code == "206" || cond_code == "207" || cond_code == "208" || cond_code == "209" || cond_code == "210" || cond_code == "211" || cond_code == "212") {
    return "S";
  }
  if (cond_code == "300" || cond_code == "301") {//阵雨
    return "T";
  }
  if (cond_code == "400" || cond_code == "404" || cond_code == "408" || cond_code == "22") { //小雪
    return "U";
  }
  if (cond_code == "407" || cond_code == "21") { //阵雪
    return "V";
  }
  if (cond_code == "401" || cond_code == "402" || cond_code == "403" || cond_code == "409" || cond_code == "410" || cond_code == "23" || cond_code == "24" || cond_code == "25") { //中雪//大雪//暴雪//中到大雪//大到暴雪
    return "W";
  }
  if (cond_code == "304" || cond_code == "405" || cond_code == "406" || cond_code == "20") { //雷阵雨伴有//冻雨//雨雪天气//阵雨夹雪
    return "X";
  }
  if (cond_code == "101" || cond_code == "4") {//多云
    return "Y";
  }
  if (cond_code == "457" || cond_code == "21" ) { //阵雪
    return "#";
  }
  if (cond_code == "313" || cond_code == "19" ) { //冻雨
    return "$";
  }
  if (cond_code == "900"  ) { //显示黑色图标不可见，雨夹雪
    return "“";
  }
  return ")";//未知
}