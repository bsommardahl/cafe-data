let promiseWithListToStream = promise =>
  promise
  |> Most.fromPromise
  |> Most.flatMap(webhooks => webhooks |> Most.fromList)
  |> Most.map(x => {
       Js.log(
         "Webhook in stream: "
         ++ (
           switch (Js.Json.stringifyAny(x)) {
           | None => "Invalid"
           | Some(json) => json
           }
         ),
       );
       x;
     });