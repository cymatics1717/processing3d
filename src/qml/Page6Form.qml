import QtQuick 2.0
import QtQuick.Window 2.0
import QtLocation 5.6
import QtPositioning 5.6

import SceneGraphRendering 1.0

SPage {
    id:page
    Plugin {
        id: mapPlugin
        name: "mapbox"
//        name: "mapboxgl"
//        name: "here"
//        name: "osm"
        // esri,  "osm", ...
        //[mapbox,mapboxgl,here,esri,itemsoverlay,osm]
        PluginParameter {
            name: "mapbox.access_token";
            value: "pk.eyJ1IjoiY3ltYXRpY3MiLCJhIjoiY2swb3pobDBxMDBpMDNvbXZqbGY1ZXo1ZCJ9.MBY9-2i4Z0UFOXbesqPngg"
        }
        PluginParameter {
            name: "mapbox.mapping.highdpi_tiles";
            value: "true"
        }
//        PluginParameter {
//            name: "here.app_id";
//            value: "R8bSivea7vu03cXmlnet"
//        }
//        PluginParameter {
//            name: "here.token";
//            value: "cz5MYhkcmULAT68M1FyOPw"
//        }
    }

    Map {
        id:mapBase
        anchors.fill: parent
        plugin: mapPlugin
        center: QtPositioning.coordinate( 40.091032332234775,116.2857597110089)

        zoomLevel: 17
        onCenterChanged: {
            console.log("center changed : "  + center.latitude + "  "+ center.longitude + "  " + center.altitude)
        }
        z: parent.z + 1
    }

    Map {
            id: mapOverlay
            anchors.fill: parent
            plugin: Plugin { name: "itemsoverlay" }
            gesture.enabled: false
            center: mapBase.center
            color: 'transparent' // Necessary to make this map transparent
            minimumFieldOfView: mapBase.minimumFieldOfView
            maximumFieldOfView: mapBase.maximumFieldOfView
            minimumTilt: mapBase.minimumTilt
            maximumTilt: mapBase.maximumTilt
            minimumZoomLevel: mapBase.minimumZoomLevel
            maximumZoomLevel: mapBase.maximumZoomLevel
            zoomLevel: mapBase.zoomLevel
            tilt: mapBase.tilt;
            bearing: mapBase.bearing
            fieldOfView: mapBase.fieldOfView
            z: mapBase.z + 1

            MapCircle {
                id: circle
                center: mapBase.center
                radius: 100
                border.width: 1
                border.color: "grey"
                color: "#660000ff"

                MouseArea {
                    anchors.fill: parent
                    drag.target: parent
                }
            }

            // The code below enables SSAA
            layer.enabled: true
            layer.smooth: true
            property int w : mapOverlay.width
            property int h : mapOverlay.height
            property int pr: Screen.devicePixelRatio
            layer.textureSize: Qt.size(w  * 2 * pr, h * 2 * pr)
        }

    ListModel {
        id: list
        ListElement { media: "/dev/video0"}
//        ListElement { media: "rtmp://58.200.131.2:1935/livetv/dftv"}
//        ListElement { media: "rtsp://wowzaec2demo.streamlock.net/vod/mp4:BigBuckBunny_115k.mov"}
//        ListElement { media: "rtsp://192.168.1.17:8554/hello"}
        ListElement { media: "http://ivi.bupt.edu.cn/hls/cctv3hd.m3u8"}
        ListElement { media: "http://ivi.bupt.edu.cn/hls/cctv6hd.m3u8"}
        ListElement { media: "file:///home/wayne/Videos/【1080P】大型人文历史纪录片《苏东坡》6集全【CCTV9-HD】 第三集：大江东去.mp4"}
        ListElement { media: "file:///home/wayne/Videos/【1080P】大型人文历史纪录片《苏东坡》6集全【CCTV9-HD】 第四集：成竹在胸.mp4"}
        ListElement { media: "file:///home/wayne/Videos/【1080P】大型人文历史纪录片《苏东坡》6集全【CCTV9-HD】 第一集：雪泥鸿爪.mp4"}
    }

    Item {
        anchors.fill: parent
        z:mapBase.z+1
        Repeater {
            model: list
            delegate: VideoSource {
                width: page.width/3
                height: page.height/3

                source: media
                MouseArea {
                    anchors.fill: parent
                    drag.target: parent
                    drag.axis: Drag.XandYAxis
                    onClicked: {
                        parent.z += 1
                    }
                    onDoubleClicked: {
                        list.remove(index)
                    }
                }
                Component.onCompleted: {
                    x = page.width * (0.6 * Math.random() + 0.1)
                    y = page.height * (0.6 * Math.random() + 0.1)
                }
            }
        }
    }
}
