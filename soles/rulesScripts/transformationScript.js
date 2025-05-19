// Ejemplo de json enviado por suela
// {
//     "id": "ASD123",
//     "sd": "right",
//     "vl": [
//         {
//         "w": 4095,
//         "p": [
//             12.2,
//             10.3
//         ]
//         },
//         {
//         "w": 4095,
//         "p": [
//             12.5,
//             11.3
//         ]
//         },
//         {
//         "w": 4095,
//         "p": [
//             13,
//             12.1
//         ]
//         },
//         {
//         "w": 4095,
//         "p": [
//             14.4,
//             13.5
//         ]
//         },
//         {
//         "w": 4095,
//         "p": [
//             15.2,
//             14.8
//         ]
//         },
//         {
//         "w": 4095,
//         "p": [
//             16.1,
//             15.9
//         ]
//         },
//         {
//         "w": 4095,
//         "p": [
//             17.3,
//             16.4
//         ]
//         },
//         {
//         "w": 3983,
//         "p": [
//             18,
//             17.2
//         ]
//         }
//     ]
//  }

var sumWeight = 0;
var sumPositionX = 0;
var sumPositionY = 0;
var xArray = [];
var yArray = [];
var higherPressureValue = msg.vl[0];
var typeOfFootprint = "";

for (var i = 0; i < msg.vl.length; i++) {
    var value = msg.vl[i];
    sumWeight += value.w;
    sumPositionX += value.p[0] * value.w;
    sumPositionY += value.p[1] * value.w;

    // populate positions array
    xArray.push(value.p[0]);
    yArray.push(value.p[1]);

    // find the high pressure
    if (value.w > higherPressureValue.w) {
        higherPressureValue = value;
    }
}

// Verificar que sumWeight no sea 0 para evitar división por 0
var centerMassX = sumWeight && sumPositionX / sumWeight;
var centerMassY = sumWeight && sumPositionY / sumWeight;

// Calcular la medida del pie
var minX = Math.min.apply(null, xArray);
var maxX = Math.max.apply(null, xArray);
var minY = Math.min.apply(null, yArray);
var maxY = Math.max.apply(null, yArray);

var footLength = maxX;
var footWidth = maxY;

// Determinar tipo de pisada
var pronationLimit = footLength * 0.3;
var supinationLimit = footLength * 0.7;

if (centerMassX < pronationLimit) {
    typeOfFootprint = "Sobrepronación";
} else if (centerMassX < pronationLimit * 1.1) {
    typeOfFootprint = "Pisada pronadora";
} else if (centerMassX > supinationLimit) {
    typeOfFootprint = "Sobresupinación";
} else if (centerMassX > supinationLimit * 0.9) {
    typeOfFootprint = "Pisada supinadora";
} else {
    typeOfFootprint = "Pisada neutra";
}

return {
    msg: {
        id: msg.id,
        sd: msg.sd,
        vl: msg.vl,
        centerOfMass: [centerMassX, centerMassY],
        footLength: footLength,
        footWidth: footWidth,
        typeOfFootprint: typeOfFootprint,
        higherPressureValue: higherPressureValue
    },
    metadata: metadata, 
    msgType: msgType
};