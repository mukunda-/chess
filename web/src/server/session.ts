"use server";

// NextJS
import { getServerSession as nextGetServerSession } from "next-auth";

// Ours
import { authOptions } from "./auth";

export async function getServerSession() {
  return await nextGetServerSession(authOptions);
}

export async function getAuthenticatedSession() {
  const session = await getServerSession();
  if (!session) {
    throw new Error("Unauthenticated access");
  }

  return session;
}
