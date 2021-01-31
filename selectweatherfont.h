
String getWeatherType(int code){
    switch (code)
    {
        case 100:
            return "晴";
            break;
        case 101:
            return "多云";
            break;
        case 102:
            return "少云";
            break;
        case 103:
            return "晴间多云";
            break;
        case 104:
            return "阴";
            break;
        case 150:
            return "晴";
            break;
        case 153:
            return "晴间多云";
            break;
        case 154:
            return "阴";
            break;
        case 300:
            return "阵雨";
            break;
        case 301:
            return "强阵雨";
            break;
        case 302:
            return "雷阵雨";
            break;
        case 303:
            return "强雷阵雨";
            break;
        case 304:
            return "冰雹";
            break;
        case 305:
            return "小雨";
            break;
        case 306:
            return "中雨";
            break;
        case 307:
            return "大雨";
            break;
        case 308:
            return "极端降雨";
            break;
        case 309:
            return "细雨";
            break;
        case 310:
            return "暴雨";
            break;
        case 311:
            return "大暴雨";
            break;
        case 312:
            return "特大暴雨";
            break;
        case 313:
            return "冻雨";
            break;
        case 314:
            return "小到中雨";
            break;
        case 315:
            return "中到大雨";
            break;
        case 316:
            return "大到暴雨";
            break;
        case 317:
            return "大暴雨";
            break;
        case 318:
            return "特大暴雨";
            break;
        case 399:
            return "雨";
            break;
        case 350:
            return "阵雨";
            break;
        case 351:
            return "强降雨";
            break;
        case 400:
            return "小雪";
            break;
        case 401:
            return "中雪";
            break;
        case 402:
            return "大雪";
            break;
        case 403:
            return "暴雪";
            break;
        case 404:
            return "雨夹雪";
            break;
        case 405:
            return "雨雪天气";
            break;
        case 406:
            return "阵雨夹雪";
            break;
        case 407:
            return "阵雪";
            break;
        case 408:
            return "小到中雪";
            break;
        case 409:
            return "中到大雪";
            break;
        case 410:
            return "大到暴雪";
            break;
        case 499:
            return "雪";
            break;
        case 456:
            return "阵雨夹雪";
            break;
        case 457:
            return "阵雪";
            break;
        case 500:
            return "薄雾";
            break;
        case 501:
            return "雾";
            break;
        case 502:
            return "霾";
            break;
        case 503:
            return "扬沙";
            break;
        case 504:
            return "浮尘";
            break;
        case 507:
            return "沙尘暴";
            break;
        case 508:
            return "强沙尘暴";
            break;
        case 509:
            return "浓雾";
            break;
        case 510:
            return "强浓雾";
            break;
        case 511:
            return "中度霾";
            break;
        case 512:
            return "重度霾";
            break;
        case 513:
            return "严重雾霾";
            break;
        case 514:
            return "大雾";
            break;
        case 515:
            return "特强浓雾";
            break;
        case 900:
            return "热";
            break;
        case 901:
            return "冷";
            break;
        case 999:
            return "未知";
            break;
        default:
            return "未知";
            break;
    }
}