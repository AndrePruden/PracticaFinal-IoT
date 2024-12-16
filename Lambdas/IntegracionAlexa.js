const Alexa = require('ask-sdk-core');
const AWS = require('aws-sdk');
const IotData = new AWS.IotData({endpoint: 'a5ji8fpy1x6e7-ats.iot.us-east-1.amazonaws.com'});
const dynamoDB = new AWS.DynamoDB.DocumentClient();

async function getThingNameFromDynamoDB() {
    const params = {
        TableName: 'MyThingData',
        Limit: 1  // Assuming you want the first item
    };

    try {
        const data = await dynamoDB.scan(params).promise();
        if (data.Items && data.Items.length > 0) {
            return data.Items[0].thing_name;
        }
        throw new Error('No thing name found in DynamoDB');
    } catch (error) {
        console.error('Error retrieving thing name:', error);
        throw error;
    }
}

async function createDynamicParams(thingName) {
    return {
        ZonaCeroParams: {
            thingName: thingName,
            payload: '{"state": {"desired": {"servoPosition": 0}}}'
        },
        ZonaUnoParams: {
            thingName: thingName,
            payload: '{"state": {"desired": {"servoPosition": 90}}}'
        },
        ZonaDosParams: {
            thingName: thingName,
            payload: '{"state": {"desired": {"servoPosition": 180}}}'
        },
        ShadowParams: {
            thingName: thingName
        }
    };
}


function getShadowPromise(params) {
    return new Promise((resolve, reject) => {
        IotData.getThingShadow(params, (err, data) => {
            if (err) {
                console.log(err, err.stack);
                reject(`Failed to get thing shadow ${err.message}`);
            } else {
                resolve(JSON.parse(data.payload));
            }
        });
    });
}

const LaunchRequestHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'LaunchRequest';
    },
    handle(handlerInput) {
        const speakOutput = 'Bienvenido al control del servo. Puedes seleccionar la zona cero, uno o dos, o consultar en que zona esta. ¿Que deseas hacer?';

        return handlerInput.responseBuilder
            .speak(speakOutput)
            .reprompt(speakOutput)
            .getResponse();
    }
};

const ZonaCeroIntentHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest'
            && Alexa.getIntentName(handlerInput.requestEnvelope) === 'ZonaCeroIntent';
    },
    async handle(handlerInput) {
        try {
            const thingName = await getThingNameFromDynamoDB();
            const params = await createDynamicParams(thingName);

            const speakOutput = 'El servo está en la Zona Cero, posición de 0 grados.';

            IotData.updateThingShadow(params.ZonaCeroParams, function(err, data) {
                if (err) console.log(err);
            });

            return handlerInput.responseBuilder
                .speak(speakOutput)
                .reprompt(speakOutput)
                .getResponse();
        } catch (error) {
            return handlerInput.responseBuilder
                .speak('Hubo un error al recuperar el nombre del dispositivo.')
                .getResponse();
        }
    }
};

const ZonaUnoIntentHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest'
            && Alexa.getIntentName(handlerInput.requestEnvelope) === 'ZonaUnoIntent';
    },
    async handle(handlerInput) {
        try {
            const thingName = await getThingNameFromDynamoDB();
            const params = await createDynamicParams(thingName);

            const speakOutput = 'El servo está en la Zona Uno, posición de 90 grados.';

            IotData.updateThingShadow(params.ZonaUnoParams, function(err, data) {
                if (err) console.log(err);
            });

            return handlerInput.responseBuilder
                .speak(speakOutput)
                .reprompt(speakOutput)
                .getResponse();
        } catch (error) {
            return handlerInput.responseBuilder
                .speak('Hubo un error al recuperar el nombre del dispositivo.')
                .getResponse();
        }
    }
};

const ZonaDosIntentHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest'
            && Alexa.getIntentName(handlerInput.requestEnvelope) === 'ZonaDosIntent';
    },
    async handle(handlerInput) {
        try {
            const thingName = await getThingNameFromDynamoDB();
            const params = await createDynamicParams(thingName);

            const speakOutput = 'El servo está en la Zona Dos, posición de 180 grados.';

            IotData.updateThingShadow(params.ZonaDosParams, function(err, data) {
                if (err) console.log(err);
            });

            return handlerInput.responseBuilder
                .speak(speakOutput)
                .reprompt(speakOutput)
                .getResponse();
        } catch (error) {
            return handlerInput.responseBuilder
                .speak('Hubo un error al recuperar el nombre del dispositivo.')
                .getResponse();
        }
    }
};

const StateIntentHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest'
            && Alexa.getIntentName(handlerInput.requestEnvelope) === 'StateIntent';
    },
    async handle(handlerInput) {
        try {
            const thingName = await getThingNameFromDynamoDB();
            const params = await createDynamicParams(thingName);

            let servoPosition = 'unknown';
            await getShadowPromise(params.ShadowParams).then((result) => {
                servoPosition = result.state.reported.servoPosition;
            });

            let speakOutput;
            if (servoPosition === 0) {
                speakOutput = 'El servo está en la Zona Cero, posición de 0 grados.';
            } else if (servoPosition === 90) {
                speakOutput = 'El servo está en la Zona Uno, posición de 90 grados.';
            } else if (servoPosition === 180) {
                speakOutput = 'El servo está en la Zona Dos, posición de 180 grados.';
            } else {
                speakOutput = 'No se pudo consultar la posición del servo, por favor intenta más tarde.';
            }

            return handlerInput.responseBuilder
                .speak(speakOutput)
                .reprompt(speakOutput)
                .getResponse();
        } catch (error) {
            return handlerInput.responseBuilder
                .speak('Hubo un error al recuperar el nombre del dispositivo.')
                .getResponse();
        }
    }
};

const HelpIntentHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest'
            && Alexa.getIntentName(handlerInput.requestEnvelope) === 'AMAZON.HelpIntent';
    },
    handle(handlerInput) {
        const speakOutput = 'Tienes las opciones de encender, apagar y consultar el estado.';

        return handlerInput.responseBuilder
            .speak(speakOutput)
            .reprompt(speakOutput)
            .getResponse();
    }
};

const CancelAndStopIntentHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest'
            && (Alexa.getIntentName(handlerInput.requestEnvelope) === 'AMAZON.CancelIntent'
                || Alexa.getIntentName(handlerInput.requestEnvelope) === 'AMAZON.StopIntent');
    },
    handle(handlerInput) {
        const speakOutput = 'Hasta pronto!';

        return handlerInput.responseBuilder
            .speak(speakOutput)
            .getResponse();
    }
};

const FallbackIntentHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest'
            && Alexa.getIntentName(handlerInput.requestEnvelope) === 'AMAZON.FallbackIntent';
    },
    handle(handlerInput) {
        const speakOutput = 'Lo siento, no entendí, intenta de nuevo.';

        return handlerInput.responseBuilder
            .speak(speakOutput)
            .reprompt(speakOutput)
            .getResponse();
    }
};

const SessionEndedRequestHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'SessionEndedRequest';
    },
    handle(handlerInput) {
        console.log(`~~~~ Session ended: ${JSON.stringify(handlerInput.requestEnvelope)}`);
        // Any cleanup logic goes here.
        return handlerInput.responseBuilder.getResponse(); // notice we send an empty response
    }
};

const IntentReflectorHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest';
    },
    handle(handlerInput) {
        const intentName = Alexa.getIntentName(handlerInput.requestEnvelope);
        const speakOutput = `Intentó ejecutar ${intentName}`;

        return handlerInput.responseBuilder
            .speak(speakOutput)
            //.reprompt('add a reprompt if you want to keep the session open for the user to respond')
            .getResponse();
    }
};

const ErrorHandler = {
    canHandle() {
        return true;
    },
    handle(handlerInput, error) {
        const speakOutput = 'Disculpa, hubo un error. Intenta de nuevo.';
        console.log(`~~~~ Error handled: ${JSON.stringify(error)}`);

        return handlerInput.responseBuilder
            .speak(speakOutput)
            .reprompt(speakOutput)
            .getResponse();
    }
};

exports.handler = Alexa.SkillBuilders.custom()
    .addRequestHandlers(
        LaunchRequestHandler,
        ZonaCeroIntentHandler,
        ZonaUnoIntentHandler,
        ZonaDosIntentHandler,
        StateIntentHandler,
        HelpIntentHandler,
        CancelAndStopIntentHandler,
        FallbackIntentHandler,
        SessionEndedRequestHandler,
        IntentReflectorHandler)
    .addErrorHandlers(
        ErrorHandler)
    .withCustomUserAgent('sample/hello-world/v1.2')
    .lambda();
