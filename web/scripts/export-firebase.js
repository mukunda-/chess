#!/usr/bin/env node

// Firebase
import admin from "firebase-admin";

// Ours - Secrets
import serviceAccount from "../secrets/service-account.json" with { type: "json" };

async function main() {
  admin.initializeApp({
    credential: admin.credential.cert({
      projectId: serviceAccount.project_id,
      privateKey: serviceAccount.private_key,
      clientEmail: serviceAccount.client_email,
    }),
  });

  const firestore = admin.firestore();

  const tasks = [];
  await firestore
    .collection("users/NEQpxujDySTM36Rw5l9fnzwSHCX2/tasks")
    .get()
    .then((snapshot) => {
      snapshot.forEach((doc) => {
        let { title, deleted, steps, status } = doc.data();
        if (deleted) {
          return;
        }

        if (steps) {
          steps = steps.join("\n");
        } else {
          steps = "";
        }

        tasks.push({
          title,
          steps,
          deleted: false,
          status: (status ?? "PENDING").toUpperCase(),
        });
      });
    });

  console.log(JSON.stringify(tasks));
}

main();
